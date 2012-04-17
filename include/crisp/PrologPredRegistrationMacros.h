#include <stdio.h>

/* PrologPredRegistrationMacros.h -----------------------------------*- C -*- */

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

/** \file \brief Preprocessor macro definitions to \e register as SWI
 *  Prolog predicates, functions that implement functionality
 *  automatically grasped from LLVM or Clang sources.
 */

#undef pl_get_one

#define pl_get_one(NAME, ARGTYPE, RESTYPE, CXXNAME)                     \
  if ( !PL_register_foreign(#ARGTYPE "::" #NAME, 2,                     \
                            (pl_function_t)                             \
                            &pl_##ARGTYPE##_##CXXNAME, 0)) {            \
    printf("%s", "Registering of " #NAME " failed.\n");                 \
    return FALSE;                                                       \
  } else {                                                              \
    printf("%s", "Registering of " #NAME " succeeded.\n");              \
  }

#undef pl_check_property

#define pl_check_property(NAME, ARGTYPE, CXXNAME)                       \
  if ( !PL_register_foreign(#ARGTYPE "::" #NAME, 1,                     \
                            (pl_function_t)                             \
                            &pl_##ARGTYPE##_##CXXNAME, 0)) {            \
    printf("%s", "Registering of " #NAME " failed.\n");                 \
    return FALSE;                                                       \
  } else {                                                              \
    printf("%s", "Registering of " #NAME " succeeded.\n");              \
  }

#undef pl_get_many

#define pl_get_many(NAME, ARGTYPE,                                      \
                    ITERTYPE, ITERBEGIN, ITEREND,                       \
                    CXXNAME)                                            \
  if ( !PL_register_foreign(#ARGTYPE "::" #NAME, 2,                     \
                            (pl_function_t)                             \
                            &pl_##ARGTYPE##_##CXXNAME,                  \
                            PL_FA_NONDETERMINISTIC)) {                  \
    printf("%s", "Registering of " #NAME " failed.\n");                 \
    return FALSE;                                                       \
  } else {                                                              \
    printf("%s", "Registering of " #NAME " succeeded.\n");              \
  }
