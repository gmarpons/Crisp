// LLVMPrologQueries.cpp ---------------------------------------------*- C++ -*-

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

#include "crisp/PrologUtilityFunctions.h"
#include "LLVMPrologQueries.h"

using namespace llvm;

namespace crisp {

  namespace prolog {

    int plReadModuleFacts(const char* FileName) {
      int Success;
      term_t FileNameA = PL_new_term_ref();
      Success = PL_put_atom_chars(FileNameA, FileName);
      if ( !Success) return Success;
      functor_t ReadModuleFactsF
        = PL_new_functor(PL_new_atom("readModuleFacts"), 1);
      term_t ReadModuleFactsT = PL_new_term_ref();
      Success = PL_cons_functor(ReadModuleFactsT, ReadModuleFactsF, FileNameA);
      if ( !Success) return Success;
      Success = PL_call(ReadModuleFactsT, NULL);
      DEBUG(if ( !Success) dbgs() << "Error calling 'readModuleFacts/1'."
                                  << "\n");
      return Success;
    }

    int plRunModuleAnalysis() {
      int Success;
      functor_t RunAnalysisF
        = PL_new_functor(PL_new_atom("run_module_analysis"), 0);
      term_t RunAnalysisT = PL_new_term_ref();
      Success = PL_cons_functor(RunAnalysisT, RunAnalysisF);
      if ( !Success) return Success;
      Success = PL_call(RunAnalysisT, NULL);
      DEBUG(if ( !Success) dbgs() << "Error calling 'run_module_analysis/0'."
                                  << "\n");
      return Success;
    }

    int plAssertModule(Module *M) {
      return plAssertIsA((void *) M, std::string("Module"));
    }

    // int plAssertLLVMFunction(Function *F) {
    //   return plAssertIsA((void *) F, std::string("LLVMFunction"));
    // }

  } // End namespace crisp::prolog

} // End namespace crisp
