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
/// \brief Crisp Clang plugin entry point.

#include <string>
#include <vector>
#include <iterator>
#include <boost/functional.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/iterator_range.hpp>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "crisp/ClangPrologQueries.h"
#include "crisp/RunPrologEngine.h"
#include "crisp/Support/Environment.h"
#include "ClangPrologPredicateRegistration.h"
#include "CompilationInfo.h"

using namespace llvm;
using namespace clang;
using namespace crisp::prolog;
namespace b = boost;
namespace l = boost::lambda;

/// \brief Main namespace for Crisp types and functions, including
/// Clang and LLVM plugins.
namespace crisp {

  class CrispConsumer : public ASTConsumer
                      , public RecursiveASTVisitor<CrispConsumer> {
  public:
    CrispConsumer(CompilerInstance &CI,
                  const std::string &BFD,
                  const std::string &RFN,
                  bool IF,
                  bool DF)
      : CompilerInstance(CI)
      , ErrorInfo()
      , BootFilesDir(BFD)
      , RulesFileName(RFN)
      , InteractiveFlag(IF)
      , DebugCrispPluginFlag(DF) {
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
    const std::string BootFilesDir;
    const std::string RulesFileName;
    const bool InteractiveFlag;
    const bool DebugCrispPluginFlag;
  };

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
#ifndef NDEBUG
    if ( !DebugFlag && DebugCrispPluginFlag) {
      DebugFlag = 1;
      dbgs() << "DebugFlag switched on." << "\n";
    }
#endif

    DEBUG(dbgs() << "Handling translation unit." << "\n");

    int Success = plRegisterPredicates();
    DEBUG(if ( !Success) dbgs() << "Error registering predicates.\n");

    // Run Prolog Engine with the boot file taken from either object (build) directory
    // (if environment variable CRISPTEST==1) or install directory (the default).
    Success = plRunEngine("PrologBootForCrispClangPlugin.sh",
                          prefixWithBuildOrInstallFullPath(BootFilesDir));
    if (Success) {
      Success = plLoadFile(RulesFileName,
                           prefixWithBuildOrInstallFullPath(PROLOG_RULES_DIR));
      DEBUG(if ( !Success) dbgs() << "Error loading rules file '"
                                  << RulesFileName << "' from '"
                                  << BootFilesDir << "'.\n");
    }

    if (Success) {
      // Traverse types in the translation unit
      b::for_each(b::make_iterator_range(Context.types_begin(),
                                         Context.types_end()),
                  l::bind(&CrispConsumer::VisitTypeFromTypesTable,this,l::_1));

      // Traverse AST to visit declarations and statements
      TraverseDecl(Context.getTranslationUnitDecl());
      DEBUG(dbgs() << "Traversing of the AST done!\n");

      // Set some global data to be accessed from Prolog (var
      // CompilationInfo defined in CompilationInfo.h).
      newCompilationInfo(CompilerInstance);

      // Main Prolog analysis
      const char* TUMainFileName = getCompilationInfo()->getMainFileName();
      Success = plRunTranslationUnitAnalysis(TUMainFileName);

      // When debugging, open a PROLOG interactive session if user asked one
      DEBUG(if (Success && InteractiveFlag) Success = plInteractiveSession());

      // Free global data
      deleteCompilationInfo();
    }

    DEBUG(if (Success) dbgs() << "Translation unit analyzed.\n";
          else dbgs() << "Translation unit analysis aborted: "
                      << "Prolog engine failed.\n";);
    (void) plCleanUp(Success ? 0 : 1); // Return value ignored

#ifndef NDEBUG
    if (DebugFlag) {
      DebugFlag = 0;
      dbgs() << "DebugFlag switched off." << "\n";
    }
#endif
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

  class CrispASTAction : public PluginASTAction {
  public:
    CrispASTAction()
        : PluginASTAction()
        , BootFilesDir("crisp-boot-dir",
                       cl::desc("Specify boot files directory"),
                       cl::value_desc("dir"),
                       cl::init(SWI_BOOT_FILES_DIR))
        , RulesFileName(cl::Positional,
                        cl::value_desc("rule_file"),
                        cl::Required)
        , FlagInteractive("-interactive",
                        cl::desc("Enable interactive Prolog session"),
                        cl::init(false))
        , FlagDebug("-debug",
                        cl::desc("Enable debug output"),
                        cl::init(false))
    { }

    // FIXME: command-line opts doesn't appear in --help

  protected:
    virtual ASTConsumer* CreateASTConsumer(CompilerInstance &CI, StringRef) {
      return new CrispConsumer(CI, BootFilesDir, RulesFileName,
                               FlagInteractive, FlagDebug);
    }

    virtual bool ParseArgs(const CompilerInstance &CI,
                           const std::vector<std::string> &Args);
  private:
    cl::opt<std::string> BootFilesDir;
    cl::opt<std::string> RulesFileName;
    cl::opt<bool> FlagInteractive;
    cl::opt<bool> FlagDebug;
  };

  bool CrispASTAction::ParseArgs(const CompilerInstance &CI,
                                 const std::vector<std::string> &Args) {

    std::vector<char const *> ArgPtrs;
    {
        // make cl::ParseCommandLineOptions happy
        static char const * const prg_name = "crisp-clang";
        ArgPtrs.push_back(prg_name);
    }

    b::transform(b::make_iterator_range(Args.begin(), Args.end()),
                 std::back_inserter(ArgPtrs),
                 b::mem_fun_ref(&std::string::c_str));

    cl::ParseCommandLineOptions(ArgPtrs.size(), &ArgPtrs.front());

    return true;
  }

  static FrontendPluginRegistry::Add<CrispASTAction>
  X("crisp-clang", "Data extraction clang plugin for CRISP");

} // End namespace crisp
