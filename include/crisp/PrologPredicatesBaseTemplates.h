// include/crisp/PrologPredicatesBaseTemplates.h ---------------------*- C++ -*-

// Copyright (C) 2011, 2012 Guillem Marpons <gmarpons@babel.ls.fi.upm.es>
//
// This file is part of Crisp.
//
// Crisp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Crisp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Crisp.  If not, see <http://www.gnu.org/licenses/>.

/// \file \brief Templates used to easily implement Prolog predicates
/// in C++ (corresponding to LLVM/Clang functions).

#ifndef PROLOGPREDICATESBASETEMPLATES_H
#define PROLOGPREDICATESBASETEMPLATES_H

#include <string>
#include <SWI-Prolog.h>

#include "llvm/ADT/Twine.h"
#include "clang/AST/Type.h"

using namespace llvm;
using namespace clang;

namespace crisp {

  namespace prolog {

    /// This function is used to concat different \c string's (or \c
    /// const \c char*, or \c StringRef's) in one single Prolog
    /// warning message.
    foreign_t pl_warning(const Twine &Message) {
      return PL_warning(Message.getSingleStringRef().data());
    }

    /// Most general template. In fact it assumes that \c ArgumentType
    /// is a pointer type.
    template <class ArgumentType>
    struct Retrieve {
      typedef ArgumentType* argument_type;

      static inline int
      _(term_t ArgumentT, argument_type* A, StringRef PredName) {
        if ( !PL_get_pointer(ArgumentT, (void **) A))
          return pl_warning(PredName + ": instantiation fault on first arg");
        return TRUE;
      }
    };

    /// Specialization for \c QualType, that is a smart pointer.
    template <>
    struct Retrieve <QualType> {
      typedef QualType argument_type;

      static inline int
      _(term_t ArgumentT, argument_type* A, StringRef PredName) {
        if ( !PL_get_pointer(ArgumentT, (void **) A))
          return pl_warning(PredName + ": instantiation fault on first arg");
        return TRUE;
      }
    };

    /// Most general template. In fact it assumes that \c ArgumentType
    /// is a pointer type and \c Getter returns a pointer.
    template <class ArgumentType,
              typename ResultType,
              ResultType (ArgumentType::* Getter)() const>
    struct Get {
      typedef const ArgumentType* argument_type;
      typedef ResultType result_type;
      static inline result_type _(argument_type A) {
        return (A ->* Getter)();
      }
    };

    /// Specialization for \c QualType (that is a smart pointer) as \c
    /// ArgumentType.
    template <typename ResultType,
              ResultType (QualType::* Getter)() const>
    struct Get<QualType, ResultType, Getter> {
      typedef QualType argument_type;
      typedef ResultType result_type;
      static inline result_type _(argument_type A) {
        return (A .* Getter)();
      }
    };

    /// Most general template. In fact it assumes that \c ResultType
    /// is a non-const pointer type.
    template <typename ResultType>
    struct Unify {
      typedef ResultType result_type;
      static inline foreign_t _(term_t ResultT, result_type Result) {
        if ( !Result) return FALSE;
        return PL_unify_pointer(ResultT, Result);
      }
    };

    /// Specialization for const pointers.
    template <typename ResultType>
    struct Unify<const ResultType*> {
      typedef const ResultType* result_type;
      static inline foreign_t _(term_t ResultT, result_type Result) {
        if ( !Result) return FALSE;
        return PL_unify_pointer(ResultT, (void *) Result); // Cast removes const
      }
    };

    /// Specialization for non-const references.
    template <typename ResultType>
    struct Unify<ResultType&> {
      typedef ResultType& result_type;
      static inline foreign_t _(term_t ResultT, result_type Result) {
        return PL_unify_pointer(ResultT, &Result);
      }
    };

    /// Specialization for const references.
    template <typename ResultType>
    struct Unify<const ResultType&> {
      typedef const ResultType& result_type;
      static inline foreign_t _(term_t ResultT, result_type Result) {
        return PL_unify_pointer(ResultT, (void *) &Result); // Cast rem. const
      }
    };

    /// Specialization for \c std::string.
    template <>
    struct Unify<std::string> {
      typedef std::string result_type;
      static inline foreign_t _(term_t ResultT, const result_type& Result) {
        return PL_unify_atom_chars(ResultT, Result.c_str());
      }
    };

    /// Specialization for \c const \c char*.
    template <>
    struct Unify<const char*> {
      typedef const char* result_type;
      static inline foreign_t _(term_t ResultT, const result_type Result) {
        if ( !Result) return FALSE;
        return PL_unify_atom_chars(ResultT, Result);
      }
    };

    /// Specialization for \c QualType, that is a smart pointer.
    template <>
    struct Unify<QualType> {
      typedef QualType result_type;
      static inline foreign_t _(term_t ResultT, const result_type& Result) {
        return PL_unify_pointer(ResultT, Result.getAsOpaquePtr());
      }
    };

    /// Specialization for \c DeclContext. It is necessary because \c
    /// DeclContext objects need to be dynamically casted into a \c
    /// Decl to be useful, as \c DeclContext doesn't extend \c Decl.
    template <>
    struct Unify<const DeclContext*> {
      typedef const DeclContext* result_type;
      static inline foreign_t _(term_t ResultT, const result_type Result) {
        if ( !Result) return FALSE;
        if ( const Decl* D = dyn_cast<Decl>(Result))
          return PL_unify_pointer(ResultT, (void *) D); // Cast removes const
        else
          return FALSE;
      }
    };

    /// Most general template. This template version is the one to be
    /// used for non-pointer types (either values or references).
    template <class ArgumentType,
              bool (ArgumentType::* Predicate)() const>
    struct Check {
      static inline foreign_t
      _(const ArgumentType& Argument) {
        return (Argument .* Predicate)() ? TRUE : FALSE;
      }
    };

    /// Specialization for pointer types.
    template <class ArgumentType,
              bool (ArgumentType::* Predicate)() const>
    struct Check <ArgumentType*, Predicate> {
      static inline foreign_t
      _(const ArgumentType* Argument) {
        return (Argument ->* Predicate)() ? TRUE : FALSE;
      }
    };

    /// General templarized function to define a binary Prolog
    /// predicate that given an individual (\c ArgumentT) and a role
    /// name (with cardinality one, or max cardinality one if it
    /// returns a pointer), unifies with the related individual (\c
    /// ResultT).
    template <typename ArgumentType,
              typename ResultType,
              ResultType (ArgumentType::* Getter)() const>
    foreign_t getOne(term_t ArgumentT, term_t ResultT, StringRef GetterName) {
      typename Retrieve<ArgumentType>::argument_type Argument;
      if ( !Retrieve<ArgumentType>::_(ArgumentT, &Argument, GetterName))
        return FALSE;
      typename Get<ArgumentType, ResultType, Getter>::result_type Result
        = Get<ArgumentType, ResultType, Getter>::_(Argument);
      return Unify<ResultType>::_(ResultT, Result);
    }

    /// General templarized function to define a unary Prolog
    /// predicate that given an individual (\c ArgumentT) and a
    /// property name, checks if the individual satisfies the
    /// property.
    template <typename ArgumentType,
              bool (ArgumentType::* Getter)() const>
    foreign_t checkProperty(term_t ArgumentT, StringRef PropertyName) {
      typename Retrieve<ArgumentType>::argument_type Argument;
      if ( !Retrieve<ArgumentType>::_(ArgumentT, &Argument, PropertyName))
        return FALSE;
      return Check<ArgumentType, Getter>::_(Argument);
    }

  } // End namespace crisp::prolog

} // End namespace crisp

#endif
