// PrologAssertClangFacts.cpp. -*- C++ -*-

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

#include "PrologAssertClangFacts.h"

using namespace llvm;
using namespace clang;

namespace prolog {

  int plAssertIsA(void *Elem, const std::string &Sort) {
    int Success;
    term_t ElemT = PL_new_term_ref();
    Success = PL_put_pointer(ElemT, Elem);
    if ( !Success) return Success;
    term_t SortA = PL_new_term_ref();
    Success = PL_put_atom_chars(SortA, Sort.c_str());
    if ( !Success) return Success;
    functor_t IsAF = PL_new_functor(PL_new_atom("isA"), 2);
    term_t IsAT = PL_new_term_ref();
    Success = PL_cons_functor(IsAT, IsAF, ElemT, SortA);
    if ( !Success) return Success;
    functor_t AssertzF = PL_new_functor(PL_new_atom("assertz"), 1);
    term_t AssertzT = PL_new_term_ref();
    Success = PL_cons_functor(AssertzT, AssertzF, IsAT);
    if ( !Success) return Success;
    Success = PL_call(AssertzT, NULL);
    DEBUG(if ( !Success) dbgs() << "Error asserting 'isA " 
                                << Sort << "' fact.\n");
    return Success;
  }

  int plAssertDeclIsA(Decl *Decl, const std::string &Sort) {
    return plAssertIsA((void *) Decl, Sort);
  }

  int plAssertTypeIsA(Type *Type, const std::string &Sort) {
    return plAssertIsA((void *) Type, Sort);
  }

  int plAssertTranslationUnitMainFileName(const char* FileName) {
    int Success;
    term_t FileNameA = PL_new_term_ref();
    Success = PL_put_atom_chars(FileNameA, FileName);
    if ( !Success) return Success;
    functor_t TranslationUnitMainFileNameF
      = PL_new_functor(PL_new_atom("translationUnitMainFileName"), 1);
    term_t TranslationUnitMainFileNameT = PL_new_term_ref();
    Success = PL_cons_functor(TranslationUnitMainFileNameT,
                              TranslationUnitMainFileNameF,
                              FileNameA);
    if ( !Success) return Success;
    functor_t AssertzF = PL_new_functor(PL_new_atom("assertz"), 1);
    term_t AssertzT = PL_new_term_ref();
    Success = PL_cons_functor(AssertzT, AssertzF, TranslationUnitMainFileNameT);
    if ( !Success) return Success;
    Success = PL_call(AssertzT, NULL);
    DEBUG(if ( !Success) dbgs()
                           << "Error asserting 'translationUnitMainFileName' "
                           << "fact.\n");
    return Success;
  }

}
