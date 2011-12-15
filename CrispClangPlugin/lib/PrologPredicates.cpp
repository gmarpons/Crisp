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
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"

#include "CompilationInfo.h"
#include "PrologPredicates.h"

using namespace clang;

namespace prolog {

  foreign_t pl_getNameAsString(term_t NamedDeclT, term_t NameT) {
    NamedDecl *ND;
    if ( !PL_get_pointer(NamedDeclT, (void **) &ND))
      return PL_warning("declName/2: instantiation fault on first arg");
    return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
  }

  foreign_t pl_unqualifiedTypeAsString(term_t TypeT, term_t NameT) {
    const Type *T;
    if ( !PL_get_pointer(TypeT, (void **) &T))
      return PL_warning("unqualifiedTypeAsString/2: "
                        "instantiation fault on first arg");
    unsigned int Qualifiers = 0;
    QualType QT(T, Qualifiers);
    return PL_unify_atom_chars(NameT, QT.getAsString().c_str());
  }

  foreign_t pl_getAsString(term_t QualTypeT, term_t NameT) {
    const QualType QT;
    if ( !PL_get_pointer(QualTypeT, (void **) &QT))
      return PL_warning("getAsString/2: "
                        "instantiation fault on first arg");
    return PL_unify_atom_chars(NameT, QT.getAsString().c_str());
  }

  // foreign_t pl_valueDeclQualTypeAsString(term_t ValueDeclT, term_t NameT) {
  //   const ValueDecl *VD;
  //   if ( !PL_get_pointer(ValueDeclT, (void **) &VD))
  //     return PL_warning("valueDeclQualTypeAsString/2: "
  //                       "instantiation fault on first arg");
  //   return PL_unify_atom_chars(NameT, VD->getType().getAsString().c_str());
  // }

  // DEPRECATED
  foreign_t pl_typeName(term_t TypeT, term_t NameT) {
    Type *T;
    if ( !PL_get_pointer(TypeT, (void **) &T))
      return PL_warning("typeName/2: instantiation fault on first arg");
    if (TypedefType *X = dyn_cast<TypedefType>(T)) {
      NamedDecl *ND = X->getDecl();
      return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
    }
    else if (TagType *X = dyn_cast<TagType>(T)) { // RecordType + EnumType
      NamedDecl *ND = X->getDecl();
      return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
    }
    else if (BuiltinType *X = dyn_cast<BuiltinType>(T)) {
      const PrintingPolicy &PP = getCompilationInfo()->getPrintingPolicy();
      return PL_unify_atom_chars(NameT, X->getName(PP));
    }
    else if (TemplateTypeParmType *X = dyn_cast<TemplateTypeParmType>(T)) {
      NamedDecl *ND = X->getDecl();
      return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
    }
    else if (UnresolvedUsingType *X = dyn_cast<UnresolvedUsingType>(T)) {
      NamedDecl *ND = X->getDecl();
      return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
    }
    else if (InjectedClassNameType *X = dyn_cast<InjectedClassNameType>(T)) {
      NamedDecl *ND = X->getDecl();
      return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
    }
    return FALSE;
  }
  
  foreign_t pl_getPointeeType(term_t PointerT, term_t PointeeT) {
    PointerType *PT;
    if ( !PL_get_pointer(PointerT, (void **) &PT))
      return PL_warning("getPointeeType/2: instantiation fault on first arg");
    const Type *Pointee = PT->getPointeeType().getTypePtr();
    return PL_unify_pointer(PointeeT, (void *) Pointee);
  }
  
  foreign_t pl_getCanonicalTypeUnqualified(term_t TypeT, term_t CanonicalT) {
    Type *T;
    if ( !PL_get_pointer(TypeT, (void **) &T))
      return PL_warning("getCanonicalTypeUnqualified/2: "
                        "instantiation fault on first arg");
    const Type *Canonical = T->getCanonicalTypeUnqualified().getTypePtr();
    return PL_unify_pointer(CanonicalT, (void *) Canonical);
  }
  
  foreign_t pl_getResultType(term_t FunctionT, term_t ResultT) {
    FunctionType *FT;
    if ( !PL_get_pointer(FunctionT, (void **) &FT))
      return PL_warning("getResultType/2: instantiation fault on first arg");
    const Type *Result = FT->getResultType().getTypePtr();
    return PL_unify_pointer(ResultT, (void *) Result);
  }
  
  foreign_t pl_getUnqualifiedType(term_t ValueT, term_t TypeT) {
    const ValueDecl *VD;
    if ( !PL_get_pointer(ValueT, (void **) &VD))
      return PL_warning("getUnqualifiedType/2: "
                        "instantiation fault on first arg");
    const Type *Type = VD->getType().getTypePtr();
    return PL_unify_pointer(TypeT, (void *) Type);
  }
  
  foreign_t pl_getType(term_t ValueT, term_t QualTypeT) {
    const ValueDecl *VD;
    if ( !PL_get_pointer(ValueT, (void **) &VD))
      return PL_warning("getType/2: instantiation fault on first arg");
    return PL_unify_pointer(QualTypeT, VD->getType().getAsOpaquePtr());
  }

  foreign_t pl_getPresumedLoc(term_t DeclT, term_t FilenameT,
                              term_t LineT, term_t ColT) {
    const Decl *D;
    if ( !PL_get_pointer(DeclT, (void **) &D))
      return PL_warning("getPresumedLoc/4: instantiation fault on first arg");
    const SourceManager &SM = getCompilationInfo()->getSourceManager();
    const PresumedLoc PL = SM.getPresumedLoc(D->getLocation());
    if ( !PL_unify_atom_chars(FilenameT, PL.getFilename())) return FALSE;
    if ( !PL_unify_int64(LineT, (int64_t) PL.getLine())) return FALSE;
    return PL_unify_int64(ColT, (int64_t) PL.getColumn());
  }     

} // End namespace prolog
