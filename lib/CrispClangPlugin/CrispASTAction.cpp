// CrispASTAction.cpp ------------------------------------------------*- C++ -*-

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

/// \file
/// Crisp Clang plugin entry point.

#include <string>
#include <vector>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/iterator_range.hpp>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "crisp/SWIPrologInterface.h"
#include "CompilationInfo.h"
#include "PrologAssertClangFacts.h"
#include "PrologRegisterPredicates.h"

using namespace llvm;
using namespace clang;
using namespace prolog;
namespace b = boost;
namespace l = boost::lambda;

namespace {

  class CrispConsumer : public ASTConsumer
                      , public RecursiveASTVisitor<CrispConsumer> {
  public:
    CrispConsumer(CompilerInstance &CI, std::string &RFN)
      : CompilerInstance(CI)
      , ErrorInfo()
      , RulesFileName(RFN) {
      // ErrorInfo is an output arg to get info about potential errors
      // opening the file/stream.
      FactsOutputStream = new raw_fd_ostream("-", ErrorInfo);
    }
    virtual ~CrispConsumer();
    virtual void HandleTranslationUnit(ASTContext &Context);
    virtual bool VisitDecl(Decl *D);

  private:
    raw_ostream &facts();
    void VisitTypeFromTypesTable(Type *T);

  private:
    CompilerInstance &CompilerInstance;
    raw_ostream *FactsOutputStream;
    std::string ErrorInfo;
    std::string RulesFileName;
  };

}

CrispConsumer::~CrispConsumer() {
  delete FactsOutputStream;
}
    
raw_ostream & CrispConsumer::facts() {
  return *FactsOutputStream;
}
    
void CrispConsumer::VisitTypeFromTypesTable(Type *T) {
  std::string TypeClassName(T->getTypeClassName());
  std::string Sort(TypeClassName + "Type");
  (void) plAssertTypeIsA(T, Sort); // Return value ignored
}

void CrispConsumer::HandleTranslationUnit(ASTContext &Context) {
  DebugFlag = 1;                // FIXME: use a plugin option to
                                // (de-)activate debug mode.

  DEBUG(dbgs() << "Handling translation unit." << "\n");
  plRegisterPredicates();
  // FIXME: RulesFileName is ignored at the moment
  int Success = plRunEngine("PrologBootForCrispClangPlugin.sh");
  
  if (Success) {
    // Get main file name
    SourceManager& SC(Context.getSourceManager());
    FileID MainFileID = SC.getMainFileID();
    const char* MainFileName = SC.getFileEntryForID(MainFileID)->getName();
    DEBUG(dbgs() << "Main source file name: " << MainFileName << "\n");

    // Traverse types in the translation unit
    b::for_each(b::make_iterator_range(Context.types_begin()
                                       , Context.types_end())
                , l::bind(&CrispConsumer::VisitTypeFromTypesTable
                          , this
                          , l::_1)
                );
    
    // Traverse AST to visit declarations and statements
    TraverseDecl(Context.getTranslationUnitDecl());
    DEBUG(dbgs() << "Traversing of the AST done!\n");
    
    // Set some global data to be accessed from Prolog (var
    // CompilationInfo defined in CompilationInfo.h).
    newCompilationInfo(CompilerInstance);

    // Main Prolog analysis
    Success = plRunTranslationUnitAnalysis(MainFileName);

    // When debugging, open a PROLOG interactive session
    if (Success) DEBUG(Success = plInteractiveSession());

    // Free global data
    deleteCompilationInfo();
  }
  
  DEBUG(if (Success) dbgs() << "Translation unit analyzed.\n";
        else dbgs() << "Translation unit analysis aborted: "
                    << "Prolog engine failed.\n";);
  (void) plCleanUp(Success ? 0 : 1); // Return value ignored

  DebugFlag = 0;                // FIXME: use a plugin option to
                                // (de-)activate debug mode.
}
    
// Visit declarations

// TODO: handle Prolog errors when visiting AST (now return values of
// pl* funcs are ignored).
    
bool CrispConsumer::VisitDecl(Decl *D) {
  // FIXME: the following code is inefficient (uses string
  // concatenation a lot of times). Specific Visit* methods should be
  // written instead.
  std::string DeclKindName(D->getDeclKindName());
  std::string Sort(DeclKindName + "Decl");
  (void) plAssertDeclIsA(D, Sort); // Return value ignored

  return true;
}

// bool CrispConsumer::VisitCXXMethodDecl(Decl *D) {
//   std::string Sort("CXXMethodDecl");
//   (void) plAssertDeclIsA(D, Sort); // Return value ignored
//   return true;
// }

namespace {

  class CrispASTAction : public PluginASTAction {
  public:
    CrispASTAction() {}
    
  protected:
    virtual ASTConsumer* CreateASTConsumer(CompilerInstance& CI
                                           , StringRef) {
      return new CrispConsumer(CI, RulesFileName);
    }
    
    virtual bool ParseArgs(const CompilerInstance &CI
                           , const std::vector<std::string> &Args);
  private:
    std::string RulesFileName;
  };
}

bool CrispASTAction::ParseArgs(const CompilerInstance &CI
                               , const std::vector<std::string> &Args) {
  // One argument needed: rules file name.
  if (Args.size() != 1) {
    DiagnosticsEngine &DE = CI.getDiagnostics();
    std::string DiagMsg = "rules file missing";
    unsigned DiagId = DE.getCustomDiagID(DiagnosticsEngine::Error, DiagMsg);
    DE.Report(DiagId);
    return false;
  }
  RulesFileName = Args[0];
  return true;
}

static FrontendPluginRegistry::Add<CrispASTAction>
X("crisp-clang", "Data extraction clang plugin for CRISP");
