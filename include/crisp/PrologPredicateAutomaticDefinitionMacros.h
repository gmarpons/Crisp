/* PrologPredicateAutomaticDefinitionMacros.h -----------------------*- C -*- */

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

/** \file \brief Preprocessor macro definitions to \e define functions
 *  that implement in C++ Prolog predicates with functionality
 *  automatically grasped from LLVM or Clang sources.
 */

#undef pl_get_one

#define pl_get_one(NAME, ARGTYPE, RESTYPE, CXXNAME)                 \
  foreign_t pl_##CXXNAME(term_t ArgumentT, term_t ResultT) {        \
    return getOne<ARGTYPE, RESTYPE, &ARGTYPE::CXXNAME>              \
      (ArgumentT, ResultT, #NAME "/2");                             \
  }

#undef pl_check_property

#define pl_check_property(NAME, ARGTYPE, CXXNAME)       \
  foreign_t pl_##CXXNAME(term_t ArgumentT) {            \
    return checkProperty<ARGTYPE, &ARGTYPE::CXXNAME>    \
      (ArgumentT, #NAME "/1");                          \
  }
