// PrologAssertLLVMFacts.cpp -----------------------------------------*- C++ -*-

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

#include <SWI-Prolog.h>

#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "PrologAssertLLVMFacts.h"

using namespace llvm;

namespace prolog {

  int plAssertLLVMModuleFileName(const char* FileName) {
    int Success;
    term_t FileNameA = PL_new_term_ref();
    Success = PL_put_atom_chars(FileNameA, FileName);
    if ( !Success) return Success;
    functor_t LLVMModuleFileNameF
      = PL_new_functor(PL_new_atom("llvmModuleFileName"), 1);
    term_t LLVMModuleFileNameT = PL_new_term_ref();
    Success = PL_cons_functor(LLVMModuleFileNameT,
                              LLVMModuleFileNameF,
                              FileNameA);
    if ( !Success) return Success;
    functor_t AssertzF = PL_new_functor(PL_new_atom("assertz"), 1);
    term_t AssertzT = PL_new_term_ref();
    Success = PL_cons_functor(AssertzT, AssertzF, LLVMModuleFileNameT);
    if ( !Success) return Success;
    Success = PL_call(AssertzT, NULL);
    DEBUG(if ( !Success) dbgs()
                           << "Error asserting 'llvmModuleFileName' "
                           << "fact.\n");
    return Success;
  }

}
