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

#include "llvm/Argument.h"
#include "llvm/Function.h"
#include "llvm/Module.h"

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

    foreign_t pl_containsArgument(term_t FuncT, term_t ArgT, control_t Handle) {
      // Arg iterators are implemented with plain pointers
      // (Argument*), so we can pass them as context to Prolog
      // avoiding dynamic memory allocation.
      typedef Function::arg_iterator ContextType;
      Function *F;
      ContextType It;

      switch (PL_foreign_control(Handle)) {
      case PL_FIRST_CALL:
        if ( !PL_get_pointer(FuncT, (void **) &F))
          return PL_warning("containsArgument/2: "
                            "instantiation fault on first arg");
        It = F->arg_begin();
        if (It == F->arg_end()) {
          return FALSE;
        } else {
          // Discard return value as unification is going to succeed
          (void) PL_unify_pointer(ArgT, (void *) It);
          PL_retry_address((void *) It);
        }
      case PL_REDO:
        if ( !PL_get_pointer(FuncT, (void **) &F))
          return PL_warning("containsArgument/2: "
                            "instantiation fault on first arg");
        It = (Argument *) PL_foreign_context_address(Handle);
        ++It;
        if (It == F->arg_end()) {
          return FALSE;
        } else {
          // Discard return value as unification is going to succeed
          (void) PL_unify_pointer(ArgT, (void *) It);
          PL_retry_address((void *) It);
        }
      default:                  // Includes case PL_PRUNED, in which
                                // nothing needs to be cleaned up.
        return TRUE;
      }
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
