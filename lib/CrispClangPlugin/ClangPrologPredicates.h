/* ClangPrologPredicates.h ------------------------------------------*- C -*- */

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
 *  \brief External (Clang) predicates to be used from Prolog.
 *
 *  This file contains C++ functions (with ISO-C headers) implementing
 *  Prolog predicates, to be called during Crisp Clang plugin
 *  execution.
 */

#ifndef CRISPCLANGPLUGIN_CLANGPROLOGPREDICATES_H
#define CRISPCLANGPLUGIN_CLANGPROLOGPREDICATES_H

#include <SWI-Prolog.h>

#ifdef __cplusplus
extern "C" {
  namespace crisp {

    namespace prolog {
#endif

      /*  If parameter types are not respected when calling one of the
          following predicates from Prolog, a runtime error is
          raised.

          Every place where a 'Type' argument is used, methods of
          'QualType' can also be used.
      */

      /* Automatically generated function declaration. */

#ifndef PARSING_WITH_EMACS_SEMANTIC
#include "crisp/PrologPredDeclarationMacros.h"
#include "ClangDeclarations.inc"
#endif

      /* Manual function declaration. */

      /** \param FunctionProtoTypeT +FunctionProtoType
       */
      foreign_t
      pl_FunctionProtoType_isConstQualified(term_t FunctionProtoTypeT);

      /** \param ArgumentT +NamedDecl
       *  \param ResultT std::string
       */
      foreign_t pl_llvmName(term_t ArgumentT, term_t ResultT);

      /** \param DeclT +Decl
       *  \param FilenameT atom
       *  \param LineT an Integer
       *  \param ColT an Integer
       */
      foreign_t pl_getPresumedLoc(term_t DeclT, term_t FilenameT,
                                  term_t LineT, term_t ColT);

      /** Uses clang diagnostic facilities to report a violation to
       *  the user.
       *  \param RuleT +atom with the name of the rule.
       *  \param DiagnosticsT +list of diagnostic messages.
       *  \param EmitLlvmFactsT +bool indicating if diagnostics
       *  need to be stored in a Prolog term.
       */
      foreign_t pl_reportViolation(term_t RuleT,
                                   term_t DiagnosticsT,
                                   term_t EmitLlvmFactsT);

#ifdef __cplusplus
    } /* End namespace crisp::prolog */

  }   /* End namespace crisp  */

} /* End "extern" C */
#endif

#endif  /* #ifndef CRISPCLANGPLUGIN_CLANGPROLOGPREDICATES_H */
