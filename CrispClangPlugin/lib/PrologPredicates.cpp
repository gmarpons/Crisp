// PrologPredicates.cpp: External predicates to be used from Prolog. -*- C++ -*-

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

#include "clang/AST/Decl.h"

#include "PrologPredicates.h"

using namespace clang;

foreign_t pl_declName(term_t DeclT, term_t NameT) {
  Decl *D;
  if (!PL_get_pointer(DeclT, (void **) &D))
    return PL_warning("declName/2: instantiation fault");
  if (NamedDecl *ND = dyn_cast<NamedDecl>(D)) {
    return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
  }
  return FALSE;
}

foreign_t pl_typeName(term_t TypeT, term_t NameT) {
  Type *T;
  if (!PL_get_pointer(TypeT, (void **) &T))
    return PL_warning("typeName/2: instantiation fault");
  if (RecordType *RT = dyn_cast<RecordType>(T)) {
    NamedDecl *ND = RT->getDecl();
    return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
  }
  else if (TypedefType *TT = dyn_cast<TypedefType>(T)) {
    NamedDecl *ND = TT->getDecl();
    return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
  }
  // TODO: add all the other Type subclasses that have a getDecl() method
  // TODO: add a special case for BuiltinType
  return FALSE;
}

foreign_t pl_getPointeeType(term_t TypeT, term_t PointeeT) {
  Type *T;
  if (!PL_get_pointer(TypeT, (void **) &T))
    return PL_warning("getPointeeType/2: instantiation fault");
  if (PointerType *PT = dyn_cast<PointerType>(T)) {
    const Type *PointeeAux = PT->getPointeeType().getTypePtr();
    int Success;
    term_t PointeeAuxT = PL_new_term_ref();
    Success = PL_put_pointer(PointeeAuxT, (void *) PointeeAux);
    return PL_unify(PointeeT, PointeeAuxT);
  }
  return FALSE;
}

foreign_t pl_getCanonicalTypeUnqualified(term_t TypeT, term_t DesugaredT) {
  Type *T;
  if (!PL_get_pointer(TypeT, (void **) &T))
    return PL_warning("getCanonicalTypeUnqualified/2: instantiation fault");
  const Type *DesugaredAux = T->getCanonicalTypeUnqualified().getTypePtr();
  int Success;
  term_t DesugaredAuxT = PL_new_term_ref();
  Success = PL_put_pointer(DesugaredAuxT, (void *) DesugaredAux);
  return PL_unify(DesugaredT, DesugaredAuxT);
}

foreign_t pl_getResultType(term_t TypeT, term_t ResultT) {
  FunctionType *FT;
  if (!PL_get_pointer(TypeT, (void **) &FT))
    return PL_warning("getResultType/2: instantiation fault");
  const Type *ResultAux = FT->getResultType().getTypePtr();
  int Success;
  term_t ResultAuxT = PL_new_term_ref();
  Success = PL_put_pointer(ResultAuxT, (void *) ResultAux);
  return PL_unify(ResultT, ResultAuxT);
}

foreign_t pl_getType(term_t ValueDeclT, term_t TypeT) {
  ValueDecl *VD;
  if ( !PL_get_pointer(ValueDeclT, (void **) &VD))
    return PL_warning("getType/2: instantiation fault");
  const Type *TypeAux = VD->getType().getTypePtr();
  int Success;
  term_t TypeAuxT = PL_new_term_ref();
  Success = PL_put_pointer(TypeAuxT, (void *) TypeAux);
  return PL_unify(TypeT, TypeAuxT);
}
