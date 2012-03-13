/* LLVMPrologPredicateRegistration.c --------------------------------*- C -*- */

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

#include "LLVMPrologPredicates.h"
#include "LLVMPrologPredicateRegistration.h"

int plRegisterPredicates() {
  /* Automatically generated function registration. */
#include "crisp/PrologPredRegistrationMacros.h"
#include "LLVMFunctions.inc"

  /* Manual function registration. */

  PL_register_foreign("getName", 2, pl_getName, 0);
  PL_register_foreign("isA_computed", 2, pl_isA_computed, 0);
  PL_register_foreign("getPointerOperand", 2, pl_getPointerOperand, 0);
  PL_register_foreign("getValueOperand", 2, pl_getValueOperand, 0);
  PL_register_foreign("getLocationFromStoreUser", 2,
                      pl_getLocationFromStoreUser, 0);
  PL_register_foreign("getLocationFromLoadUser", 2,
                      pl_getLocationFromLoadUser, 0);
  PL_register_foreign("createLocation", 2, pl_createLocation, 0);
  PL_register_foreign("aliasLessThanNoAlias", 2,
                      pl_aliasLessThanNoAlias, 0);
  PL_register_foreign("alias", 3, pl_alias, 0);
  PL_register_foreign("getFunction", 3, pl_getFunction, 0);
  return TRUE;
}
