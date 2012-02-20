/* LLVMPrologPredicates.h -------------------------------------------*- C -*- */

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
 *  \brief External (LLVM) predicates to be used from Prolog.
 *
 *  This file contains C++ functions (with ISO-C headers) implementing
 *  Prolog predicates, to be called during Crisp LLVM Pass execution.
 */

#ifndef CRISPLLVMPASS_CLANGPROLOGPREDICATES_H
#define CRISPLLVMPASS_CLANGPROLOGPREDICATES_H

#include <SWI-Prolog.h>

#ifdef __cplusplus
extern "C" {
  namespace crisp {

    namespace prolog {
#endif

      /*  If parameter types are not respected when calling one of the
          following predicates from Prolog, a runtime error is
          raised.
      */

      /** \param ValueT +Value
       *  \param NameT Atom
       */
      foreign_t pl_getName(term_t ValueT, term_t NameT);

      /** \param FunctionT +Function
       *  \param ArgumentT Argument
       */
      foreign_t pl_containsArgument(term_t FuncT, term_t ArgT,
                                    control_t Handle);

      /** \param ModuleT +Module
       *  \param NameT +Atom
       *  \param FunctionT Function
       */
      foreign_t pl_getFunction(term_t ModuleT, term_t NameT,
                               term_t FunctionT);

#ifdef __cplusplus
    } /* End namespace crisp::prolog */

  }   /* End namespace crisp  */

} /* End "extern" C */
#endif

#endif  /* #ifndef CRISPLLVMPASS_CLANGPROLOGPREDICATES_H */
