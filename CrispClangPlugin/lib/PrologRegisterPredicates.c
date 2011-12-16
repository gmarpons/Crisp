/* PrologRegisterPredicates.cpp: Prolog predicate registering func. -*- C -*- */

/* Copyright (C) 2011 Guillem Marpons <gmarpons@babel.ls.fi.upm.es>

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

#include "PrologPredicates.h"
#include "PrologRegisterPredicates.h"

void plRegisterPredicates() {
  PL_register_foreign("getAsString", 2, 
                      (pl_function_t) &pl_getAsString, 0);
  PL_register_foreign("getCanonicalType", 2
                      , (pl_function_t) &pl_getCanonicalType, 0);
  PL_register_foreign("isConstQualified", 1
                      , (pl_function_t) &pl_isConstQualified, 0);
  PL_register_foreign("isConstFunctionProtoType", 1
                      , (pl_function_t) &pl_isConstFunctionProtoType, 0);
  PL_register_foreign("getPointeeType", 2
                      , (pl_function_t) &pl_getPointeeType, 0);
  PL_register_foreign("getResultType", 2
                      , (pl_function_t) &pl_getResultType, 0);
  PL_register_foreign("getPresumedLoc", 4,
                      (pl_function_t) &pl_getPresumedLoc, 0);
  PL_register_foreign("getNameAsString", 2, 
                      (pl_function_t) &pl_getNameAsString, 0);
  PL_register_foreign("getType", 2
                      , (pl_function_t) &pl_getType, 0);
}
