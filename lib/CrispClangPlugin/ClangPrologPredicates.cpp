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

#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/raw_ostream.h"

#include "crisp/PrologPredicatesBaseTemplates.h"
#include "CompilationInfo.h"
#include "ClangPrologPredicates.h"

using namespace clang;

namespace crisp {

  namespace prolog {

    // Automatically generated function definition.

#include "crisp/PrologPredicateAutomaticDefinitionMacros.h"
#include "ClangFunctions.inc"

    // Manual function definition.

    // foreign_t pl_getDeclContext(term_t ArgumentT, term_t ResultT) {
    //   return getOne<Decl, const DeclContext*, &Decl::getDeclContext>
    //     (ArgumentT, ResultT, "declContext/2");
    // }

    // foreign_t pl_getNonClosureContext(term_t ArgumentT, term_t ResultT) {
    //   return getOne<Decl, const DeclContext*, &Decl::getNonClosureContext>
    //     (ArgumentT, ResultT, "nonClosureContext/2");
    // }

    // foreign_t pl_getASTContext(term_t ArgumentT, term_t ResultT) {
    //   return getOne<Decl, ASTContext&, &Decl::getASTContext>
    //     (ArgumentT, ResultT, "ASTContext/2");
    // }

    // foreign_t pl_getDeclKindName(term_t ArgumentT, term_t ResultT) {
    //   return getOne<Decl, const char *, &Decl::getDeclKindName>
    //     (ArgumentT, ResultT, "declKindName/2");
    // }

    // foreign_t pl_getTranslationUnitDecl(term_t ArgumentT, term_t ResultT) {
    //   return getOne<ASTContext, TranslationUnitDecl*,
    //                 &ASTContext::getTranslationUnitDecl>
    //     (ArgumentT, ResultT, "translationUnitDecl/2");
    // }

    foreign_t pl_getAsString(term_t ArgumentT, term_t ResultT) {
      return getOne<QualType, std::string, &QualType::getAsString>
        (ArgumentT, ResultT, "asString/2");
    }

    foreign_t pl_getCanonicalType(term_t ArgumentT, term_t ResultT) {
      return getOne<QualType, QualType, &QualType::getCanonicalType>
        (ArgumentT, ResultT, "canonicalType/2");
    }

    // foreign_t pl_isConstQualified(term_t ArgumentT) {
    //   return checkProperty<QualType, &QualType::isConstQualified>
    //     (ArgumentT, "constQualified/1");
    // }

    foreign_t pl_FunctionProtoType_isConstQualified(term_t ArgumentT) {
      Retrieve<FunctionProtoType>::argument_type Argument;
      if ( !Retrieve<FunctionProtoType>::
           _(ArgumentT, &Argument, "FunctionProtoType_constQualified/1"))
        return FALSE;
      Qualifiers Q = Qualifiers::fromCVRMask(Argument->getTypeQuals());
      return Check<Qualifiers, &Qualifiers::hasConst>::_(Q);
    }

    foreign_t pl_getPointeeType(term_t ArgumentT, term_t ResultT) {
      return getOne<PointerType, QualType, &PointerType::getPointeeType>
        (ArgumentT, ResultT, "pointeeType/2");
    }

    foreign_t pl_getResultType(term_t ArgumentT, term_t ResultT) {
      return getOne<FunctionType, QualType, &FunctionType::getResultType>
        (ArgumentT, ResultT, "resultType/2");
    }

    foreign_t pl_getNameAsString(term_t ArgumentT, term_t ResultT) {
      return getOne<NamedDecl, std::string, &NamedDecl::getNameAsString>
        (ArgumentT, ResultT, "nameAsString/2");
    }

    foreign_t pl_getType(term_t ArgumentT, term_t ResultT) {
      return getOne<ValueDecl, QualType, &ValueDecl::getType>
        (ArgumentT, ResultT, "getType/2");
    }

    foreign_t pl_mangleName(term_t ArgumentT, term_t ResultT) {
      Retrieve<FunctionDecl>::argument_type Argument;
      if ( !Retrieve<FunctionDecl>::
           _(ArgumentT, &Argument, "FunctionDecl_mangleName/2")) return FALSE;
      std::string StreamString, Result;
      llvm::raw_string_ostream Stream(StreamString);
      MangleContext *MC = getCompilationInfo()->getMangleContext();
      MC->mangleName(Argument, Stream);
      Result = Stream.str();
      return Unify<std::string>::_(ResultT, Result);
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

  } // End namespace crisp::prolog

} // End namespace crisp
