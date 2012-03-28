/* DeclExtractorPrologPredicateRegistration.c -----------------------*- C -*- */

/* Copyright (C) 2011, 2012 Guillem Marpons <gmarpons@babel.ls.fi.upm.es>

   This file is part of Crisp.

   Crisp is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Crisp is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Crisp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "DeclExtractorPrologPredicates.h"
#include "DeclExtractorPrologPredicateRegistration.h"

int plRegisterPredicates() {
  /* Automatically generated function registration. */
#include "crisp/PrologPredRegistrationMacros.h"
#include "ClangFunctionsForBootstrapping.inc"

  /* Manual function registration. */
  if ( !PL_register_foreign("CXXBaseSpecifier::baseDecl", 2,
                            (pl_function_t) &pl_CXXBaseSpecifier_getBaseDecl,
                            0))
    return FALSE;
  if ( !PL_register_foreign("CXXMethodDecl::constQualified", 1,
                            (pl_function_t) &pl_CXXMethodDecl_isConstQualified,
                            0))
    return FALSE;

  return TRUE;
}
