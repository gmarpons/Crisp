// ClangPrologQueries.cpp --------------------------------------------*- C++ -*-

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

#include "crisp/PrologUtilityFunctions.h"
#include "ClangPrologQueries.h"

using namespace llvm;
using namespace clang;

namespace crisp {

  namespace prolog {

    int plAssertDeclIsA(Decl *Decl, const std::string &Sort) {
      return plAssertIsA((void *) Decl, Sort);
    }

    int plAssertTypeIsA(Type *Type, const std::string &Sort) {
      return plAssertIsA((void *) Type, Sort);
    }

    int plRunTranslationUnitAnalysis(const char* FileName) {
      int Success;
      term_t FileNameA = PL_new_term_ref();
      Success = PL_put_atom_chars(FileNameA, FileName);
      if ( !Success) return Success;
      functor_t RunAnalysisF
        = PL_new_functor(PL_new_atom("runTranslationUnitAnalysis"), 1);
      term_t RunAnalysisT = PL_new_term_ref();
      Success = PL_cons_functor(RunAnalysisT, RunAnalysisF, FileNameA);
      if ( !Success) return Success;
      Success = PL_call(RunAnalysisT, NULL);
      DEBUG(if ( !Success) dbgs()
                             << "Error calling 'runTranslationUnitAnalysis/1'."
                             << "\n");
      return Success;
    }

  } // End namespace crisp::prolog

} // End namespace crisp
