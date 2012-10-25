// CompilationInfo.h -------------------------------------------------*- C++ -*-

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
/// \brief Global data to be used by Prolog predicates implemented in C++.

#ifndef CRISPCLANGPLUGIN_COMPILATIONINFO_H
#define CRISPCLANGPLUGIN_COMPILATIONINFO_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/Mangle.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/Basic/FileManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

namespace crisp {

  namespace prolog {

    class CompilationInfo {
    public:
      const CompilerInstance& getCompilerInstance() const;

      const LangOptions& getLangOpts() const;

      const PrintingPolicy& getPrintingPolicy() const;

      const SourceManager& getSourceManager() const;

      MangleContext* getMangleContext();

      const char* getMainFileName();

      void setNormalDiagnosticConsumer();

      void setLlvmDiagnosticConsumer();

      llvm::raw_ostream& getLlvmDiagsOStream();

      friend void newCompilationInfo(CompilerInstance &CI);
      friend void deleteCompilationInfo();

    private:
      CompilationInfo(CompilerInstance &CI)
        : CompilerInstance(CI)
        , LangOptions(CI.getLangOpts())
        , PrintingPolicy(LangOptions)
        , SourceManager(CI.getSourceManager())
        , MangleContext(CI.getASTContext().createMangleContext())
        , MainFileName(SourceManager
                       .getFileEntryForID(SourceManager.getMainFileID())->getName())
        , LlvmDiagsOStream((MainFileName + Twine(".diags")).str().c_str(),
                           ErrorInfoDiags)
        , NormalDiagnosticConsumer(*CI.getDiagnostics().takeClient())
        , LlvmDiagnosticConsumer(LlvmDiagsOStream, &CI.getDiagnosticOpts())
      {
        DEBUG(llvm::dbgs() << "Main source file name: "<< MainFileName << "\n");

        if ( !ErrorInfoDiags.empty())
          llvm::report_fatal_error(Twine("Error opening file ")
                                   + Twine(MainFileName) + Twine(".diags"));
        if ( !ErrorInfoFacts.empty())
          llvm::report_fatal_error(Twine("Error opening file ")
                                   + Twine(MainFileName) + Twine(".pl"));

        LlvmDiagnosticConsumer.BeginSourceFile(CI.getLangOpts(),
                                               &CI.getPreprocessor());
      }

      ~CompilationInfo();

      const CompilerInstance &CompilerInstance;
      const LangOptions &LangOptions;
      const PrintingPolicy PrintingPolicy;
      const SourceManager &SourceManager;
      MangleContext *MangleContext;
      const char* MainFileName;
      std::string ErrorInfoDiags, ErrorInfoFacts;
      llvm::raw_fd_ostream LlvmDiagsOStream;
      // llvm::raw_fd_ostream LlvmFactsOStream;
      DiagnosticConsumer &NormalDiagnosticConsumer;
      TextDiagnosticPrinter LlvmDiagnosticConsumer;
    };

    CompilationInfo* getCompilationInfo();

    void newCompilationInfo(CompilerInstance &CI);

    void deleteCompilationInfo();

    inline const CompilerInstance&
    CompilationInfo::getCompilerInstance() const {
      return CompilerInstance;
    }

    inline const LangOptions& CompilationInfo::getLangOpts() const {
      return LangOptions;
    }

    inline const PrintingPolicy& CompilationInfo::getPrintingPolicy() const {
      return PrintingPolicy;
    }

    inline const SourceManager& CompilationInfo::getSourceManager() const {
      return SourceManager;
    }

    inline MangleContext* CompilationInfo::getMangleContext() {
      return MangleContext;
    }

    inline const char* CompilationInfo::getMainFileName() {
      return MainFileName;
    }

    inline void CompilationInfo::setNormalDiagnosticConsumer() {
      DiagnosticsEngine &DE = CompilerInstance.getDiagnostics();
      if (DE.getClient() != &NormalDiagnosticConsumer)
        DE.setClient(&NormalDiagnosticConsumer, false); // doesn't own client
    }

    inline void CompilationInfo::setLlvmDiagnosticConsumer() {
      DiagnosticsEngine &DE = CompilerInstance.getDiagnostics();
      if (DE.getClient() != &LlvmDiagnosticConsumer)
        DE.setClient(&LlvmDiagnosticConsumer, false); // doesn't own client
    }

    inline llvm::raw_ostream& CompilationInfo::getLlvmDiagsOStream() {
      return LlvmDiagsOStream;
    }

  } // End namespace crisp::prolog

} // End namespace crisp

#endif
