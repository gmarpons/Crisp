/* ClangPrologPredicateRegistration.c -------------------------------*- C -*- */

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

#include "ClangPrologPredicates.h"
#include "ClangPrologPredicateRegistration.h"

int plRegisterPredicates() {

  /* Automatically generated function registration. */
#include "crisp/PrologPredRegistrationMacros.h"
#include "ClangDeclarations.inc"

  /* Manual function registration. */
  PL_register_foreign("isConstFunctionProtoType", 1,
                      (pl_function_t) &pl_FunctionProtoType_isConstQualified,
                      0);
  PL_register_foreign("mangleName", 2,
                      (pl_function_t) &pl_mangleName, 0);
  PL_register_foreign("report_violation", 3,
                      (pl_function_t) &pl_reportViolation, 0);

  return TRUE;
}
