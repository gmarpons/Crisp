// DeclExtractorASTAction.cpp ----------------------------------------*- C++ -*-

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
#include <vector>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/iterator_range.hpp>
#include <SWI-Prolog.h>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"

#include "crisp/ClangPrologQueries.h"
#include "crisp/RunPrologEngine.h"
#include "DeclExtractorPrologPredicateRegistration.h"

using namespace llvm;
using namespace clang;
using namespace crisp::prolog;
namespace b = boost;
namespace l = boost::lambda;

namespace crisp {

  class DeclExtractorConsumer
    : public ASTConsumer
    , public RecursiveASTVisitor<DeclExtractorConsumer> {
  public:
    DeclExtractorConsumer(CompilerInstance &CI)
      : CompilerInstance(CI) {
    }
    virtual ~DeclExtractorConsumer();
    virtual void HandleTranslationUnit(ASTContext &Context);
    virtual bool VisitCXXMethodDecl(CXXMethodDecl *D);
    virtual bool VisitCXXRecordDecl(CXXRecordDecl *D);

  private:
    void VisitTypeFromTypesTable(Type *T);

  private:
    CompilerInstance &CompilerInstance;
  };

  DeclExtractorConsumer::~DeclExtractorConsumer() {
  }

  void DeclExtractorConsumer::VisitTypeFromTypesTable(Type *T) {
    std::string TypeClassName(T->getTypeClassName());
    std::string Sort(TypeClassName + "Type");
    (void) plAssertTypeIsA(T, Sort); // Return value ignored
  }

  void DeclExtractorConsumer::HandleTranslationUnit(ASTContext &Context) {
    DebugFlag = 1;                // FIXME: use a plugin option to
                                  // (de-)activate debug mode.

    DEBUG(dbgs() << "Handling translation unit." << "\n");
    plRegisterPredicates();

    int Success
      = plRunEngine("PrologBootForDeclExtractor.sh");

    if (Success) {
      // Get main file name
      SourceManager& SC(Context.getSourceManager());
      FileID MainFileID = SC.getMainFileID();
      const char* MainFileName = SC.getFileEntryForID(MainFileID)->getName();
      DEBUG(dbgs() << "Main source file name: " << MainFileName << "\n");

      // Traverse types in the translation unit
      // b::for_each(b::make_iterator_range(Context.types_begin()
      //                                    , Context.types_end())
      //             , l::bind(&DeclExtractorConsumer::VisitTypeFromTypesTable
      //                       , this
      //                       , l::_1)
      //             );

      // Traverse AST to visit declarations and statements
      TraverseDecl(Context.getTranslationUnitDecl());
      DEBUG(dbgs() << "Traversing of the AST done!\n");

      // Main Prolog analysis
      Success = plRunTranslationUnitAnalysis(MainFileName);

      // When debugging, open a PROLOG interactive session
      if (Success) DEBUG(Success = plInteractiveSession());
    }

    DEBUG(if (Success) dbgs() << "Translation unit analyzed.\n";
          else dbgs() << "Translation unit analysis aborted: "
                      << "Prolog engine failed.\n";);
    (void) plCleanUp(Success ? 0 : 1); // Return value ignored

    DebugFlag = 0;                // FIXME: use a plugin option to
                                  // (de-)activate debug mode.
  }

  // Visit declarations

  // bool DeclExtractorConsumer::VisitDecl(Decl *D) {
  //   // FIXME: the following code is inefficient (uses string
  //   // concatenation a lot of times). Specific Visit* methods should be
  //   // written instead.
  //   std::string DeclKindName(D->getDeclKindName());
  //   std::string Sort(DeclKindName + "Decl");
  //   (void) plAssertDeclIsA(D, Sort); // Return value ignored

  //   return true;
  // }

  bool DeclExtractorConsumer::VisitCXXMethodDecl(CXXMethodDecl *D) {
    std::string Sort("CXXMethodDecl");
    (void) plAssertDeclIsA(D, Sort);
    return true;
  }

  bool DeclExtractorConsumer::VisitCXXRecordDecl(CXXRecordDecl *D) {
    std::string Sort("CXXrecordDecl");
    (void) plAssertDeclIsA(D, Sort);
    return true;
  }

  class DeclExtractorASTAction : public PluginASTAction {
  public:
    DeclExtractorASTAction() {}

  protected:
    virtual ASTConsumer* CreateASTConsumer(CompilerInstance& CI, StringRef) {
      return new DeclExtractorConsumer(CI);
    }

    virtual bool ParseArgs(const CompilerInstance &CI,
                           const std::vector<std::string> &Args);
  };

  bool DeclExtractorASTAction::ParseArgs(const CompilerInstance &CI,
                                         const std::vector<std::string> &Args) {
    return true;
  }

  static FrontendPluginRegistry::Add<DeclExtractorASTAction>
  X("decl-extractor", "Declaration extraction clang plugin");

} // End namespace crisp
