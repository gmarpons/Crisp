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
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/raw_ostream.h"

#include "crisp/PrologPredTemplatesForClangTypes.h"
#include "CompilationInfo.h"
#include "ClangPrologPredicates.h"

using namespace clang;

namespace crisp {

  namespace prolog {

    // Automatically generated function definitions.

#ifndef PARSING_WITH_EMACS_SEMANTIC
#include "crisp/PrologPredDefinitionMacros.h"
#include "ClangDeclarations.inc"
#endif

    // Manual function definition.

    foreign_t pl_FunctionProtoType_isConstQualified(term_t ArgumentT) {
      Retrieve<FunctionProtoType>::argument_type Argument;
      if ( !Retrieve<FunctionProtoType>::
           _(ArgumentT, &Argument, "FunctionProtoType_constQualified/1"))
        return FALSE;
      Qualifiers Q = Qualifiers::fromCVRMask(Argument->getTypeQuals());
      return Check<Qualifiers, &Qualifiers::hasConst>::_(Q);
    }

    foreign_t pl_llvmName(term_t ArgumentT, term_t ResultT) {
      Retrieve<NamedDecl>::argument_type Argument;
      if ( !Retrieve<NamedDecl>::
           _(ArgumentT, &Argument, "NamedDecl_llvmName/2")) return FALSE;
      MangleContext *MC = getCompilationInfo()->getMangleContext();
      std::string Result;
      if ( !MC->shouldMangleDeclName(Argument)) {
        Result = Argument->getNameAsString();
        return Unify<std::string>::_(ResultT, Result);
      }
      // Name needs mangling
      std::string StreamString;
      llvm::raw_string_ostream Stream(StreamString);
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

    enum ItemKind { ItemKind_Decl, ItemKind_NamedDecl, ItemKind_Stmt,
                    ItemKind_Type, ItemKind_Null, ItemKind_Invalid };

    /// Add an item's name as part of a diagnostic. The item must be
    /// either a \c NamedDecl or a \c Type.
    ///
    /// \param DB Diagnostic we're building.
    ///
    /// \param ItemT Prolog term representing the item and its kind.
    foreign_t addItemNameToDiagnostic(DiagnosticBuilder &DB, term_t ItemT) {
      atom_t ItemKindA = PL_new_atom("");
      int Arity;
      if ( !PL_get_name_arity(ItemT, &ItemKindA, &Arity)) return FALSE;
      const char* ItemKindChars;
      if ( !(ItemKindChars = PL_atom_chars(ItemKindA)))
        return FALSE;
      ItemKind ItemKind = StringSwitch<crisp::prolog::ItemKind>(ItemKindChars)
        .Case("NamedDecl", ItemKind_NamedDecl)
        .Case("Type", ItemKind_Type)
        .Default(ItemKind_Invalid);
      assert(ItemKind != ItemKind_Invalid
             && "unexpected item kind functor!");
      term_t ItemPointerT = PL_new_term_ref();
      if ( !PL_get_arg(1, ItemT, ItemPointerT)) return FALSE;
      SourceLocation SrcLoc;
      if (ItemKind == ItemKind_NamedDecl) {
        const NamedDecl *ND;
        if ( !PL_get_pointer(ItemPointerT, (void **) &ND)) return FALSE;
        DB << ND->getDeclName();
      } else {                  // ItemKind == ItemKind_Type
        // TODO: ItemKind_Type case
      }

      return TRUE;
    }

    /// \c DiagnosticT has format
    /// FUNCTOR(FormatString,ItemForSrcLoc,ListOfItems,ItemForSrcRange),
    /// where FUNCTOR is one of \c warn or \c note, and items are one
    /// of \c Decl/1, \c NamedDecl/1, \c Stmt/1, \c Type/1, or \c
    /// Null/0 (the last one only to indicate an inexistent source
    /// range). Not all operations are possible on any item kind.
    foreign_t processDiagnostic(const char* Rule, term_t DiagnosticT) {
      // FIXME: all 'return FALSE' should be PL_warning'
      atom_t DiagKindA = PL_new_atom(""); // 'warn' or 'note'
      int Arity;
      if ( !PL_get_name_arity(DiagnosticT, &DiagKindA, &Arity)) return FALSE;
      const char* DiagKindChars;
      if ( !(DiagKindChars = PL_atom_chars(DiagKindA)))
        return FALSE;
      DiagnosticsEngine::Level Level =
        StringSwitch<DiagnosticsEngine::Level>(DiagKindChars)
        .Case("warn", DiagnosticsEngine::Warning)
        .Case("note", DiagnosticsEngine::Note)
        // Dirty hack: no "invalid" value for this enum
        .Default(DiagnosticsEngine::Ignored);
      assert(Level != DiagnosticsEngine::Ignored
             && "unexpected message kind functor!");

      term_t ArgT = PL_new_term_ref(); // Reused for each argument

      // Arg 1: message format string
      if ( !PL_get_arg(1, DiagnosticT, ArgT)) return FALSE;
      char *FormatString;
      // TODO: Check if our assumtion that being FormatStrinT and
      // atom, BUF_RING store option is enough to avoid a memory copy,
      // is correct.
      if ( !PL_get_chars(ArgT, &FormatString, CVT_ATOM|BUF_RING))
        return FALSE;

      // Arg 2: item for SourceLocation
      // Possible item kinds: Decl, Stmt.
      if ( !PL_get_arg(2, DiagnosticT, ArgT)) return FALSE;
      atom_t ItemKindA = PL_new_atom("");
      if ( !PL_get_name_arity(ArgT, &ItemKindA, &Arity)) return FALSE;
      const char* ItemKindChars;
      if ( !(ItemKindChars = PL_atom_chars(ItemKindA))) return FALSE;
      ItemKind ItemKind = StringSwitch<crisp::prolog::ItemKind>(ItemKindChars)
        .Case("Decl", ItemKind_Decl)
        .Case("Stmt", ItemKind_Stmt)
        .Default(ItemKind_Invalid);
      assert(ItemKind != ItemKind_Invalid && "unexpected item kind functor!");
      term_t ItemPointerT = PL_new_term_ref();
      if ( !PL_get_arg(1, ArgT, ItemPointerT)) return FALSE;
      SourceLocation SrcLoc;
      if (ItemKind == ItemKind_Decl) {
        const Decl *D;
        if ( !PL_get_pointer(ItemPointerT, (void **) &D)) return FALSE;
        SrcLoc = D->getLocStart();
      } else {                  // ItemKind == ItemKind_Stmt
        const Stmt *S;
        if ( !PL_get_pointer(ItemPointerT, (void **) &S)) return FALSE;
        SrcLoc = S->getLocStart();
      }

      // Diagnostic generation
      const CompilerInstance &CI = getCompilationInfo()->getCompilerInstance();
      DiagnosticsEngine &DE = CI.getDiagnostics();
      Twine Msg = Twine(Rule) + Twine(": ") + Twine(FormatString);
      unsigned DiagId = DE.getCustomDiagID(Level, Msg.str());
      DiagnosticBuilder DB = DE.Report(SrcLoc, DiagId);

      // Arg 3: list of items
      if ( !PL_get_arg(3, DiagnosticT, ArgT)) return FALSE;
      term_t HeadT = PL_new_term_ref();
      term_t ListT = PL_copy_term_ref(ArgT); // Copy, as we need to write
      foreign_t Ok = TRUE;
      while (Ok && PL_get_list(ListT, HeadT, ListT)) {
        Ok = addItemNameToDiagnostic(DB, HeadT);
      }

      // Arg 4: (optional) item for source range.
      // Possible item kinds: Decl, Stmt. Null to indicate no range.
      if ( !PL_get_arg(4, DiagnosticT, ArgT)) return FALSE;
      if ( !PL_get_name_arity(ArgT, &ItemKindA, &Arity)) return FALSE;
      if ( !(ItemKindChars = PL_atom_chars(ItemKindA))) return FALSE;
      ItemKind = StringSwitch<crisp::prolog::ItemKind>(ItemKindChars)
        .Case("Decl", ItemKind_Decl)
        .Case("Stmt", ItemKind_Stmt)
        .Case("Null", ItemKind_Null)
        .Default(ItemKind_Invalid);
      assert(ItemKind != ItemKind_Invalid && "unexpected item kind functor!");
      if (ItemKind != ItemKind_Null) {
        if ( !PL_get_arg(1, ArgT, ItemPointerT)) return FALSE;
        SourceRange SrcRange;
        if (ItemKind == ItemKind_Decl) {
          const Decl *D;
          if ( !PL_get_pointer(ItemPointerT, (void **) &D)) return FALSE;
          SrcRange = D->getSourceRange();
        } else {                // ItemKind == ItemKind_Stmt
          const Stmt *S;
          if ( !PL_get_pointer(ItemPointerT, (void **) &S)) return FALSE;
          SrcRange = S->getSourceRange();
        }
        DB << SrcRange;
      }

      // Emit the diagnostic and return
      DB.~DiagnosticBuilder();
      return TRUE;
    }

    foreign_t pl_reportViolation(term_t RuleT, term_t DiagnosticsT) {
      char *Rule;
      if ( !PL_get_atom_chars(RuleT, &Rule)) return FALSE;

      term_t HeadT = PL_new_term_ref();
      term_t ListT = PL_copy_term_ref(DiagnosticsT); // Copy, we need to write
      foreign_t Ok = TRUE;
      while (Ok && PL_get_list(ListT, HeadT, ListT)) {
        Ok = processDiagnostic(Rule, HeadT);
      }

      return TRUE;
    }

  } // End namespace crisp::prolog

} // End namespace crisp
