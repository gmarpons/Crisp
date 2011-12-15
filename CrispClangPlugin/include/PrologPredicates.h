/* PrologPredicates.h: External predicates to be used from Prolog. -*- C -*- */

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

#ifndef CRISPCLANGPLUGIN_PROLOGPREDICATES_H
#define CRISPCLANGPLUGIN_PROLOGPREDICATES_H

#include <SWI-Prolog.h>

#ifdef __cplusplus
extern "C" {
  namespace prolog {
#endif

    /** If parameter types are not respected when calling one of the
        following predicates from Prolog, a runtime error is
        raised.
    */

    /** \param QualTypeT +QualType
        \param NameT Atom
     */
    foreign_t pl_getAsString(term_t QualTypeT, term_t NameT);

    /** \param QualTypeT +QualType
        \param TypeT Type
     */
    foreign_t pl_getTypePtr(term_t QualTypeT, term_t TypeT);

    /** \param TypeT +Type
        \param NameT Atom
     */
    foreign_t pl_unqualifiedTypeAsString(term_t TypeT, term_t NameT);

    /** \param TypeT +Type
        \param CanonicalT Type
     */
    foreign_t pl_getCanonicalTypeUnqualified(term_t TypeT, term_t CanonicalT);

    /** \param PointerT +PointerType
        \param PointeeT QualType
     */
    foreign_t pl_getPointeeType(term_t PointerT, term_t PointeeT);

    /** \param FunctionT +FunctionType
        \param ResultT QualType
     */
    foreign_t pl_getResultType(term_t FunctionT, term_t ResultT);

    /** \param DeclT +Decl
        \param FilenameT Atom
        \param LineT an Integer
        \param ColT an Integer
     */
    foreign_t pl_getPresumedLoc(term_t DeclT, term_t FilenameT,
                                term_t LineT, term_t ColT);

    /** \param NamedDeclT +NamedDecl
        \param NameT Atom
     */
    foreign_t pl_getNameAsString(term_t NamedDeclT, term_t NameT);

    /** \param ValueT +ValueDecl
        \param TypeT QualType
     */
    foreign_t pl_getType(term_t ValueT, term_t TypeT);

#ifdef __cplusplus
  } /* End namespace prolog */
}
#endif

#endif
