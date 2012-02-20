// LLVMPrologPredicates.cpp ------------------------------------------*- C++ -*-

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

#include <string>

#include "llvm/Argument.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/Module.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Use.h"

#include "LLVMPrologPredicates.h"

using namespace llvm;

namespace crisp {

  namespace prolog {

    foreign_t pl_getName(term_t ValueT, term_t NameT) {
      Value *V;
      if ( !PL_get_pointer(ValueT, (void **) &V))
        return PL_warning("getName/2: instantiation fault on first arg");
      return PL_unify_atom_chars(NameT, V->getName().str().c_str());
    }

    /// \param PredicateName <doc>
    /// \param ContainerT <doc>
    /// \param ElemT <doc>
    /// \param Handle <doc>
    /// \return <doc>
    /// This template works only for <tt>IteratorType</tt>s
    /// implemented as plain pointers, to avoid dynamic memory
    /// allocation.
    template< typename ContainerType,
              typename ElemType,
              typename IteratorType,
              IteratorType (ContainerType::* BeginFunc) (),
              IteratorType (ContainerType::* EndFunc) ()
              >
    foreign_t contains(const char* PredicateName,
                       term_t ContainerT, term_t ElemT, control_t Handle) {
      ContainerType *C;         // SWI-Prolog API doesn't support constness
      IteratorType It;

      switch (PL_foreign_control(Handle)) {
      case PL_FIRST_CALL:
        // Get container
        if ( !PL_get_pointer(ContainerT, (void **) &C)) {
          std::string WarningStr(PredicateName);
          WarningStr += "/2 instantiation fault on first arg";
          return PL_warning(WarningStr.c_str());
        }

        // Get first contained element
        It = (C ->* BeginFunc)();

        // Return next contained element (when exists)
        if (It == (C ->* EndFunc)()) {
          return FALSE;
        } else {
          // Discard return value as unification is going to succeed
          (void) PL_unify_pointer(ElemT, (void *) It);
          PL_retry_address((void *) It);
        }
      case PL_REDO:
        // Get container
        if ( !PL_get_pointer(ContainerT, (void **) &C)) {
          std::string WarningStr(PredicateName);
          WarningStr += "/2 instantiation fault on first arg";
          return PL_warning(WarningStr.c_str());
        }

        // Get next contained element
        It = (ElemType *) PL_foreign_context_address(Handle);
        ++It;

        // Return next contained element (when exists)
        if (It == (C ->* EndFunc)()) {
          return FALSE;
        } else {
          // Discard return value as unification is going to succeed
          (void) PL_unify_pointer(ElemT, (void *) It);
          PL_retry_address((void *) It);
        }
      case PL_PRUNED:           // Nothing needs to be cleaned up
      default:
        return TRUE;
      }
    }

    template< typename ContainerType,
              typename ElemType,
              typename IteratorType,
              IteratorType (* BeginFunc) (ContainerType *),
              IteratorType (* EndFunc) (ContainerType *)
              >
    foreign_t contains(const char* PredicateName,
                       term_t ContainerT, term_t ElemT, control_t Handle) {
      ContainerType *C;         // SWI-Prolog API doesn't support constness
      IteratorType *Context;
      IteratorType It;

      switch (PL_foreign_control(Handle)) {
      case PL_FIRST_CALL:
        // Get container
        if ( !PL_get_pointer(ContainerT, (void **) &C)) {
          std::string WarningStr(PredicateName);
          WarningStr += "/2 instantiation fault on first arg";
          return PL_warning(WarningStr.c_str());
        }

        // Get first contained element
        It = BeginFunc(C);

        // Return next contained element (when exists)
        if (It == EndFunc(C)) {
          return FALSE;
        } else {
          // Discard return value as unification is going to succeed
          (void) PL_unify_pointer(ElemT, (void *) &(*It));
          Context = new IteratorType(It);
          PL_retry_address((void *) Context);
        }
      case PL_REDO:
        // Get container
        if ( !PL_get_pointer(ContainerT, (void **) &C)) {
          std::string WarningStr(PredicateName);
          WarningStr += "/2 instantiation fault on first arg";
          return PL_warning(WarningStr.c_str());
        }

        // Get next contained element
        Context = (IteratorType *) PL_foreign_context_address(Handle);
        It = *Context;
        ++It;

        // Return next contained element (when exists)
        if (It == EndFunc(C)) {
          return FALSE;
        } else {
          // Discard return value as unification is going to succeed
          (void) PL_unify_pointer(ElemT, (void *) &(*It));
          *Context = It;
          PL_retry_address((void *) Context);
        }
      case PL_PRUNED:
        Context = (IteratorType *) PL_foreign_context_address(Handle);
        if (Context) delete Context;
      default:
        return TRUE;
      }
    }

    foreign_t pl_containsArgument(term_t FuncT, term_t ArgT, control_t Handle) {
      return contains< Function, Argument, Function::arg_iterator,
                       &Function::arg_begin, &Function::arg_end >
        ("containsArgument", FuncT, ArgT, Handle);
    }

    foreign_t pl_containsInstruction(term_t FuncT, term_t InstT,
                                     control_t Handle) {
      return contains< Function, Instruction, inst_iterator,
                       &inst_begin, &inst_end >
        ("containsInstruction", FuncT, InstT, Handle);
    }

    foreign_t pl_containsOp(term_t UserT, term_t UseT, control_t Handle) {
      return contains< User, Use, User::op_iterator,
                       &User::op_begin, &User::op_end >
        ("containsOp", UserT, UseT, Handle);
    }

    foreign_t pl_get(term_t UseT, term_t ValueT) {
      const Use *U;
      if ( !PL_get_pointer(UseT, (void **) &U))
        return PL_warning("get/2: instantiation fault on first arg");
      return PL_unify_pointer(ValueT, U->get());
    }

    foreign_t pl_getFunction(term_t ModuleT, term_t NameT,
                             term_t FunctionT) {
      const Module *M;
      if ( !PL_get_pointer(ModuleT, (void **) &M))
        return PL_warning("getFunction/3: instantiation fault on first arg");
      char *N;
      if ( !PL_get_atom_chars(NameT, &N))
        return PL_warning("getFunction/3: instantiation fault on second arg");
      return PL_unify_pointer(FunctionT, M->getFunction(StringRef(N)));
    }

  } // End namespace crisp::prolog

} // End namespace crisp
