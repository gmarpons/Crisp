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

#include "crisp/PrologPredicateAutomaticDeclarationMacros.h"
#include "ClangFunctions.inc"

      /* Manual function declaration. */

      /** \param TypeT +Type
       *  \param NameT Atom
       */
      foreign_t pl_getAsString(term_t TypeT, term_t NameT);

      /** \param TypeT +Type
       *  \param CanonicalT Type
       */
      foreign_t pl_getCanonicalType(term_t TypeT, term_t CanonicalT);

      /** \param TypeT +Type
       */
      /* foreign_t pl_isConstQualified(term_t TypeT); */

      /** \param FunctionProtoTypeT +FunctionProtoType
       */
      foreign_t
      pl_FunctionProtoType_isConstQualified(term_t FunctionProtoTypeT);

      /** \param PointerT +PointerType
       *  \param PointeeT Type
       */
      foreign_t pl_getPointeeType(term_t PointerT, term_t PointeeT);

      /** \param FunctionT +FunctionType
       *  \param ResultT Type
       */
      foreign_t pl_getResultType(term_t FunctionT, term_t ResultT);

      /** \param DeclT +Decl
       *  \param FilenameT Atom
       *  \param LineT an Integer
       *  \param ColT an Integer
       */
      foreign_t pl_getPresumedLoc(term_t DeclT, term_t FilenameT,
                                  term_t LineT, term_t ColT);

      /** \param NamedDeclT +NamedDecl
       *  \param NameT Atom
       */
      foreign_t pl_getNameAsString(term_t NamedDeclT, term_t NameT);

      /** \param ValueT +ValueDecl
       *  \param TypeT Type
       */
      foreign_t pl_getType(term_t ValueT, term_t TypeT);

      /** \param FunctionT +VarDecl or +FunctionDecl (not a
       *  CXXConstructorDecl or CXXDestructorDecl
       *  \param MangledNameT Atom
       */
      foreign_t pl_mangleName(term_t FunctionT, term_t MangledNameT);

#ifdef __cplusplus
    } /* End namespace crisp::prolog */

  }   /* End namespace crisp  */

} /* End "extern" C */
#endif

#endif  /* #ifndef CRISPCLANGPLUGIN_CLANGPROLOGPREDICATES_H */
