// ClangPrologPredicates.cpp -----------------------------------------*- C++ -*-

// Copyright (C) 2011, 2012 Guillem Marpons <gmarpons@babel.ls.fi.upm.es>
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

#include <string>

#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/raw_ostream.h"

#include "CompilationInfo.h"
#include "ClangPrologPredicates.h"

using namespace clang;

namespace crisp {

  namespace prolog {

    foreign_t pl_getAsString(term_t TypeT, term_t NameT) {
      const QualType QT;
      if ( !PL_get_pointer(TypeT, (void **) &QT))
        return PL_warning("getAsString/2: "
                          "instantiation fault on first arg");
      return PL_unify_atom_chars(NameT, QT.getAsString().c_str());
    }

    foreign_t pl_getCanonicalType(term_t TypeT, term_t CanonicalT) {
      QualType QT;
      if ( !PL_get_pointer(TypeT, (void **) &QT))
        return PL_warning("getCanonicalType/2: "
                          "instantiation fault on first arg");
      return PL_unify_pointer(CanonicalT,
                              QT.getCanonicalType().getAsOpaquePtr());
    }

    foreign_t pl_isConstQualified(term_t TypeT) {
      QualType QT;
      if ( !PL_get_pointer(TypeT, (void **) &QT))
        return PL_warning("isConstQualified/1: "
                          "instantiation fault on first arg");
      return QT.isConstQualified() ? TRUE : FALSE;
    }

    foreign_t pl_isConstFunctionProtoType(term_t FunctionProtoTypeT) {
      const FunctionProtoType *FT;
      if ( !PL_get_pointer(FunctionProtoTypeT, (void **) &FT))
        return PL_warning("isConstFunctionProtoType/1: "
                          "instantiation fault on first arg");
      Qualifiers Q = Qualifiers::fromCVRMask(FT->getTypeQuals());
      return Q.hasConst() ? TRUE : FALSE;
    }

    foreign_t pl_getPointeeType(term_t PointerT, term_t PointeeT) {
      PointerType *PT;
      if ( !PL_get_pointer(PointerT, (void **) &PT))
        return PL_warning("getPointeeType/2: instantiation fault on first arg");
      return PL_unify_pointer(PointeeT, PT->getPointeeType().getAsOpaquePtr());
    }

    foreign_t pl_getResultType(term_t FunctionT, term_t ResultT) {
      FunctionType *FT;
      if ( !PL_get_pointer(FunctionT, (void **) &FT))
        return PL_warning("getResultType/2: instantiation fault on first arg");
      return PL_unify_pointer(ResultT, FT->getResultType().getAsOpaquePtr());
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

    foreign_t pl_getNameAsString(term_t NamedDeclT, term_t NameT) {
      NamedDecl *ND;
      if ( !PL_get_pointer(NamedDeclT, (void **) &ND))
        return PL_warning("declName/2: instantiation fault on first arg");
      return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
    }

    foreign_t pl_getType(term_t ValueT, term_t TypeT) {
      const ValueDecl *VD;
      if ( !PL_get_pointer(ValueT, (void **) &VD))
        return PL_warning("getType/2: instantiation fault on first arg");
      return PL_unify_pointer(TypeT, VD->getType().getAsOpaquePtr());
    }

    foreign_t pl_mangleName(term_t FunctionT, term_t MangledNameT) {
      const FunctionDecl *FD;
      if ( !PL_get_pointer(FunctionT, (void **) &FD))
        return PL_warning("mangleName/2: instantiation fault on first arg");
      std::string StreamString, MangledName;
      llvm::raw_string_ostream Stream(StreamString);
      MangleContext *MC = getCompilationInfo()->getMangleContext();
      MC->mangleName(FD, Stream);
      MangledName = Stream.str();
      // std::cout << "\n" << MangledName << "\n";
      return PL_unify_atom_chars(MangledNameT, MangledName.c_str());
    }

  } // End namespace crisp::prolog

} // End namespace crisp
