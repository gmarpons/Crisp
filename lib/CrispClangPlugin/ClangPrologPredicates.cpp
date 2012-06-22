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
#include "clang/AST/DeclFriend.h"
// #include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Expr.h"
// #include "clang/AST/LocInfoType.h" // Probably not needed
#include "clang/AST/Stmt.h"
#include "clang/AST/StmtCXX.h"
// #include "clang/AST/StmtObjC.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/raw_ostream.h"

#include "crisp/PrologPredTemplatesForClangTypes.h"
#include "CompilationInfo.h"
#include "ClangPrologPredicates.h"

using namespace clang;

namespace crisp {

  namespace prolog {

    // Automatically generated function definitions.

#include "crisp/PrologPredDefinitionMacros.h"
#include "ClangDeclarations.inc"

    // Manual function definition.

    foreign_t pl_FunctionProtoType_isConstQualified(term_t ArgumentT) {
      Retrieve<FunctionProtoType>::argument_type Argument;
      if ( !Retrieve<FunctionProtoType>::
           _(ArgumentT, &Argument, "FunctionProtoType_constQualified/1"))
        return FALSE;
      Qualifiers Q = Qualifiers::fromCVRMask(Argument->getTypeQuals());
      return Check<Qualifiers, &Qualifiers::hasConst>::_(Q);
    }

    foreign_t pl_mangleName(term_t ArgumentT, term_t ResultT) {
      Retrieve<FunctionDecl>::argument_type Argument;
      if ( !Retrieve<FunctionDecl>::
           _(ArgumentT, &Argument, "FunctionDecl_mangleName/2")) return FALSE;
      std::string StreamString, Result;
      llvm::raw_string_ostream Stream(StreamString);
      MangleContext *MC = getCompilationInfo()->getMangleContext();
      if (const CXXConstructorDecl *C =dyn_cast<CXXConstructorDecl>(Argument)) {
        MC->mangleCXXCtor(C, Ctor_Complete, Stream);
      } else if
          (const CXXDestructorDecl *D =dyn_cast<CXXDestructorDecl>(Argument)) {
        MC->mangleCXXDtor(D, Dtor_Complete, Stream);
      } else {
        MC->mangleName(Argument, Stream);
      }
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

    foreign_t pl_reportViolation(term_t RuleT, term_t MsgT, term_t CulpritsT) {
      // FIXME: all 'return FALSE' should be PL_warning'
      const char *Rule;
      if ( !PL_get_atom_chars(RuleT, (char **) &Rule)) return FALSE;
      const char *Msg;
      if ( !PL_get_atom_chars(MsgT, (char **) &Msg)) return FALSE;
      // atom_t StmtA = PL_new_atom("stmt");
      // functor_t StmtF = PL_new_functor(StmtA, 1);
      atom_t NamedDeclA = PL_new_atom("NamedDecl");
      functor_t NamedDeclF = PL_new_functor(NamedDeclA, 2);

      // functor_t SortF;
      // if ( !PL_get_functor(LocT, &SortF)) return FALSE;
      // term_t ElemT = PL_new_term_ref();
      // if ( !PL_get_arg(1, LocT, ElemT)) return FALSE;
      // SourceLocation SL;
      // if ( PL_unify_functor(LocT, StmtF)) {
      //   Stmt *S;
      //   if ( !PL_get_pointer(ElemT, (void **) &S)) return FALSE;
      //   SL = S->getLocStart();
      // }
      // // FIXME: same for Decl and other elems.

      const CompilerInstance &CI = getCompilationInfo()->getCompilerInstance();
      DiagnosticsEngine &DE = CI.getDiagnostics();
      Twine MsgWithRule = Twine(Rule) + Twine(": ") + Twine(Msg);
      unsigned DiagId = DE.getCustomDiagID(DiagnosticsEngine::Warning,
                                           MsgWithRule.str());
      DiagnosticBuilder DB = DE.Report(DiagId);

      term_t HeadT = PL_new_term_ref();
      term_t ListT = PL_copy_term_ref(CulpritsT); // copy as we need to write
      while(PL_get_list(ListT, HeadT, ListT)) {
        term_t ElemT = PL_new_term_ref();
        if ( !PL_get_arg(1, HeadT, ElemT)) return FALSE;
        if ( PL_unify_functor(HeadT, NamedDeclF)) {
          const NamedDecl *ND;
          if ( !PL_get_pointer(ElemT, (void **) &ND)) return FALSE;
          DB << ND->getDeclName();
          continue;
        }
        // FIXME: same for Type and other elems
      }
      DB.~DiagnosticBuilder();  // Emits the diagnostic

      ListT = PL_copy_term_ref(CulpritsT);
      while(PL_get_list(ListT, HeadT, ListT)) {
        functor_t SortF;
        if ( !PL_get_functor(HeadT, &SortF)) return FALSE;
        term_t ElemT = PL_new_term_ref();
        if ( !PL_get_arg(1, HeadT, ElemT)) return FALSE;
        term_t MsgT = PL_new_term_ref();
        if ( !PL_get_arg(2, HeadT, MsgT)) return FALSE;
        const char *Msg;
        if ( !PL_get_atom_chars(MsgT, (char **) &Msg)) return FALSE;
        if ( PL_unify_functor(HeadT, NamedDeclF)) {
          const NamedDecl *ND;
          if ( !PL_get_pointer(ElemT, (void **) &ND)) return FALSE;
          DiagId = DE.getCustomDiagID(DiagnosticsEngine::Note, Msg);
          DiagnosticBuilder DB = DE.Report(ND->getLocStart(), DiagId);
          DB << ND->getDeclName();
          DB.~DiagnosticBuilder(); // Emits the diagnostic
          continue;
        }
        // FIXME: same for Type and other elems
      }

      return TRUE;
    }

  } // End namespace crisp::prolog

} // End namespace crisp
