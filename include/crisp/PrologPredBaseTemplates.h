// include/crisp/PrologPredBaseTemplates.h ---------------------------*- C++ -*-

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

#ifndef PROLOGPREDBASETEMPLATES_H
#define PROLOGPREDBASETEMPLATES_H

#include <iterator>
#include <string>
#include <SWI-Prolog.h>

#include "llvm/ADT/ilist.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Use.h"

using namespace llvm;

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

    /// Most general template, only used for value-types.
    template <typename ResultType>
    struct Unify {
      typedef ResultType result_type;
      static inline foreign_t _(term_t ResultT, result_type Result) {
        return PL_unify_pointer(ResultT, &Result);
      }
    };

    /// Specialization for non-const pointers.
    template <typename ResultType>
    struct Unify<ResultType*> {
      typedef ResultType* result_type;
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
    /// name (\c Getter, with cardinality one, or max cardinality one
    /// if it returns a pointer), unifies with the related individual
    /// (\c ResultT).
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
    /// property name (\c Getter), checks if the individual satisfies
    /// the property.
    template <typename ArgumentType,
              bool (ArgumentType::* Getter)() const>
    foreign_t checkProperty(term_t ArgumentT, StringRef PropertyName) {
      typename Retrieve<ArgumentType>::argument_type Argument;
      if ( !Retrieve<ArgumentType>::_(ArgumentT, &Argument, PropertyName))
        return FALSE;
      return Check<ArgumentType, Getter>::_(Argument);
    }

    /// Most general template.
    template <typename IteratorType,
              typename UnifyType>
    struct UnifyIteratorAux {
      typedef IteratorType iterator_type;
      static inline foreign_t _(term_t ResultT, iterator_type I) {
        return PL_unify_pointer(ResultT, (void *) &*I);
      }
    };

    /// Specialization for pointer types.
    template <typename IteratorType,
              typename UnifyType>
    struct UnifyIteratorAux<IteratorType, UnifyType*> {
      typedef IteratorType iterator_type;
      static inline foreign_t _(term_t ResultT, iterator_type I) {
        return PL_unify_pointer(ResultT, (void *) *I);
      }
    };

    /// Specialization for llvm::Use (kind of smart pointer to a
    /// Value*).
    template <typename IteratorType>
    struct UnifyIteratorAux<IteratorType, Use> {
      typedef IteratorType iterator_type;
      static inline foreign_t _(term_t ResultT, iterator_type I) {
        return PL_unify_pointer(ResultT, (Value *) *I);
      }
    };

    /// Most general template.
    template <typename IteratorType>
    struct UnifyIterator {
      typedef IteratorType iterator_type;
      typedef std::iterator_traits<iterator_type> traits_type;
      typedef typename traits_type::value_type unify_type;
      static inline foreign_t _(term_t ResultT, iterator_type I) {
        return UnifyIteratorAux<iterator_type, unify_type>::_(ResultT, I);
      }
    };

    /// Specialization for pointer types.
    template <typename IteratorType>
    struct UnifyIterator<IteratorType*> {
      typedef IteratorType* iterator_type;
      typedef std::iterator_traits<iterator_type> traits_type;
      typedef typename traits_type::value_type unify_type;
      static inline foreign_t _(term_t ResultT, iterator_type I) {
        if ( !I) return FALSE;
        return UnifyIteratorAux<iterator_type, unify_type>::_(ResultT, I);
      }
    };

    /// Specialization for llvm::ilist_iterator.
    template <typename ValueType>
    struct UnifyIterator< ilist_iterator<ValueType> > {
      typedef ilist_iterator<ValueType> iterator_type;
      typedef std::iterator_traits<iterator_type> traits_type;
      typedef typename traits_type::pointer unify_type;
      static inline foreign_t _(term_t ResultT, iterator_type I) {
        if ( !((unify_type) I)) return FALSE;
        return PL_unify_pointer(ResultT, (void *) (unify_type) I);
      }
    };

    /// The \c const_* version of the \c IteratorType must be always
    /// given.
    template <typename ContainerType,
              typename IteratorType,
              IteratorType (ContainerType::* Begin)() const,
              IteratorType (ContainerType::* End)() const>
    struct MemberIteratorHelper {
      typedef const ContainerType* container_type;
      typedef IteratorType iterator_type;
      static inline iterator_type begin(container_type C) {
        return (C ->* Begin)();
      }
      static inline iterator_type end(container_type C) {
        return (C ->* End)();
      }
    };

    /// The \c const_* version of the \c IteratorType must be always
    /// given.
    template <typename ContainerType,
              typename IteratorType,
              IteratorType (* Begin)(const ContainerType*),
              IteratorType (* End)(const ContainerType*)>
    struct ExternalIteratorHelper {
      typedef const ContainerType* container_type;
      typedef IteratorType iterator_type;
      static inline iterator_type begin(container_type C) {
        return Begin(C);
      }
      static inline iterator_type end(container_type C) {
        return End(C);
      }
    };

    /// Most general version for template class \c Context. It doesn't
    /// assume any specific property about the iterator type used, so
    /// it takes the most general policy to store context information:
    /// use the heap. More specific and efficient storing policies
    /// should be used whenever possible.
    template <typename IteratorType>
    struct Context {
      typedef IteratorType iterator_type;
      typedef IteratorType* context_type;
      static inline context_type newContext(iterator_type I) {
        return new iterator_type(I);
      }
      static inline void deleteContext(context_type C) {
        delete C;
      }
      static inline void context2iter(context_type C, iterator_type& I) {
        I = *C;
      }
      static inline void iter2context(iterator_type I, context_type& C) {
        *C = I;
      }
    };

    /// Specialization for \c ilist_iterator 's. As class \c
    /// ilist_iterator is in fact a smart pointer, we don't need
    /// dynamic memory allocation in this case.
    template <typename ValueType>
    struct Context < ilist_iterator<ValueType> > {
      typedef ilist_iterator<ValueType> iterator_type;
      typedef typename ilist_iterator<ValueType>::pointer context_type;
      static inline context_type newContext(iterator_type I) {
        return I;
      }
      static inline void deleteContext(context_type C) {
        // Do nothing.
      }
      static inline void context2iter(context_type C, iterator_type& I) {
        I = C;
      }
      static inline void iter2context(iterator_type I, context_type& C) {
        C = I;
      }
    };

    /// Specialization for pointer types. We don't need dynamic memory
    /// allocation in this case.
    template <typename ValueType>
    struct Context<ValueType*> {
      typedef ValueType* iterator_type;
      typedef iterator_type context_type;
      static inline context_type newContext(iterator_type I) {
        return I;
      }
      static inline void deleteContext(context_type C) {
        // Do nothing.
      }
      static inline void context2iter(context_type C, iterator_type& I) {
        I = C;
      }
      static inline void iter2context(iterator_type I, context_type& C) {
        C = I;
      }
    };

    /// General templarized function to define a binary Prolog
    /// predicate that given an individual (\c ArgumentT) and a role
    /// name (\c IteratorType) non-deterministically unifies \c
    /// ResultT with all the individuals related with \c ArgumentT
    /// through this role.
    template <typename ArgumentType,
              typename IteratorHelper>
    foreign_t getManyAux(term_t ArgumentT, term_t ResultT, StringRef PredName,
                         control_t Handle) {
      typedef typename IteratorHelper::iterator_type iterator_type;
      typedef typename Context<iterator_type>::context_type context_type;
      iterator_type It;
      context_type Ctxt;
      typename Retrieve<ArgumentType>::argument_type Container;

      switch (PL_foreign_control(Handle)) {
      case PL_FIRST_CALL: case PL_REDO:       // Get Container
        if ( !Retrieve<ArgumentType>::_(ArgumentT, &Container, PredName))
          return FALSE;
      }

      switch (PL_foreign_control(Handle)) {
      case PL_FIRST_CALL:                     // Get first elem
        It = IteratorHelper::begin(Container);
        Ctxt = Context<iterator_type>::newContext(It);
        break;
      case PL_REDO:                           // Get next elem
        Ctxt = (context_type) PL_foreign_context_address(Handle);
        Context<iterator_type>::context2iter(Ctxt, It);
        Context<iterator_type>::iter2context(++It, Ctxt);
        break;
      case PL_PRUNED:
        Ctxt = (context_type) PL_foreign_context_address(Handle);
      }

      switch (PL_foreign_control(Handle)) {
      case PL_FIRST_CALL: case PL_REDO:       // Return first/next (when exists)
        if (It == IteratorHelper::end(Container)) return FALSE;
        else {
          (void) UnifyIterator<iterator_type>::_(ResultT, It);
          PL_retry_address((void *) Ctxt);    // Returns. Cast remove constness
        }
      case PL_PRUNED:                         // Clean context
        Context<iterator_type>::deleteContext(Ctxt);
      default:
        return TRUE;
      }
    }

    /// Overloaded function template. Version for begin/end member
    /// functions.
    template <typename ArgumentType,
              typename IteratorType,
              IteratorType (ArgumentType::* Begin)() const,
              IteratorType (ArgumentType::* End)() const>
    foreign_t getMany(term_t ArgumentT, term_t ResultT, StringRef PredName,
                      control_t Handle) {
      typedef MemberIteratorHelper<ArgumentType, IteratorType, Begin, End>
        IteratorHelper;
      return getManyAux<ArgumentType, IteratorHelper>
        (ArgumentT, ResultT, PredName, Handle);
    }

    /// Overloaded function template. Version for begin/end ordinary
    /// functions.
    template <typename ArgumentType,
              typename IteratorType,
              IteratorType (* Begin)(const ArgumentType*),
              IteratorType (* End)(const ArgumentType*)>
    foreign_t getMany(term_t ArgumentT, term_t ResultT, StringRef PredName,
                      control_t Handle) {
      typedef ExternalIteratorHelper<ArgumentType, IteratorType, Begin, End>
        IteratorHelper;
      return getManyAux<ArgumentType, IteratorHelper>
        (ArgumentT, ResultT, PredName, Handle);
    }

  } // End namespace crisp::prolog

} // End namespace crisp

#endif
