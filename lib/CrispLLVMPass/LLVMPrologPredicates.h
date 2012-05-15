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

      /* Automatically generated function declaration. */

#include "crisp/PrologPredDeclarationMacros.h"
#include "LLVMDeclarations.inc"
      /* Extra function (not a member function) */
      pl_get_many(instruction, Function, const_inst_iterator, inst_begin, inst_end)

      /* Manual function declaration. */

      /** \param InstT +Instruction
       *  \param SortT Sort
       */
      foreign_t pl_isA_computed(term_t InstT, term_t SortT);

      /** \param StoreT +StoreInst
       *  \param LocationT -AliasAnalysis::Location
       */
      foreign_t pl_getLocationFromStoreUser(term_t StoreT, term_t LocationT);

      /** \param LoadT +LoadInst
       *  \param LocationT -AliasAnalysis::Location
       */
      foreign_t pl_getLocationFromLoadUser(term_t LoadT, term_t LocationT);

      /** \param ValueT +Value that has pointer type
       *  \param LocationT -AliasAnalysis::Location
       */
      foreign_t pl_createLocation(term_t ValueT, term_t LocationT);

      /** \param LocationT1 +Location
       *  \param LocationT2 +Location
       */
      foreign_t pl_aliasLessThanNoAlias(term_t LocationT1, term_t LocationT2);

      /** \param LocationT1 +Location
       *  \param LocationT2 +Location
       *  \param AliasT integer
       */
      foreign_t pl_alias(term_t LocationT1, term_t LocationT2, term_t AliasT);

      /** \param ModuleT +Module
       *  \param NameT +Atom
       *  \param FunctionT Function
       */
      foreign_t pl_getFunction(term_t ModuleT, term_t NameT,
                               term_t FunctionT);

      foreign_t pl_reportViolationLLVM(term_t RuleT, term_t MsgT,
                                       term_t CulpritsT);

#ifdef __cplusplus
    } /* End namespace crisp::prolog */

  }   /* End namespace crisp  */

} /* End "extern" C */
#endif

#endif  /* #ifndef CRISPLLVMPASS_CLANGPROLOGPREDICATES_H */
