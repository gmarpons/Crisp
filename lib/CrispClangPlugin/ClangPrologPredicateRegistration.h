/* ClangPrologPredicateRegistration.h -------------------------------*- C -*- */

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

/** \file
 *  \brief Prolog predicate registering function for Clang Plugin.
 */

#ifndef CRISPCLANGPLUGIN_CLANGPROLOGPREDICATEREGISTRATION_H
#define CRISPCLANGPLUGIN_CLANGPROLOGPREDICATEREGISTRATION_H

#include <SWI-Prolog.h>

#ifdef __cplusplus
extern "C" {
#endif

  void plRegisterPredicates();

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* ifndef CRISPCLANGPLUGIN_CLANGPROLOGPREDICATEREGISTRATION_H */
