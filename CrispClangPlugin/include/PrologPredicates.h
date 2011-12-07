// PrologPredicates.h: External predicates to be used from Prolog. -*- C -*-
//
// Copyright (C) 2011 Guillem Marpons <gmarpons@babel.ls.fi.upm.es>
//
// This file is part of Crisp.
//
// Crisp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Crisp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Crisp.  If not, see <http://www.gnu.org/licenses/>.

#include <SWI-Prolog.h>

#ifdef __cplusplus
extern "C" {
#endif

  foreign_t pl_declName(term_t DeclT, term_t NameT);
  foreign_t pl_typeName(term_t TypeT, term_t NameT);
  foreign_t pl_getPointeeType(term_t TypeT, term_t PointeeT);
  foreign_t pl_getCanonicalTypeUnqualified(term_t TypeT, term_t DesugaredT);
  foreign_t pl_getResultType(term_t TypeT, term_t ResultT);
  foreign_t pl_getType(term_t ValueDeclT, term_t TypeT);

#ifdef __cplusplus
}
#endif
