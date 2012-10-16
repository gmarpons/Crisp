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
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
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

      // DiagnosticConsumer& getLlvmFactsDiagnosticConsumer();

      void setNormalDiagnosticConsumer();

      void setLlvmFactsDiagnosticConsumer();

      friend void newCompilationInfo(CompilerInstance &CI);
      friend void deleteCompilationInfo();

    private:
      CompilationInfo(CompilerInstance &CI, llvm::raw_ostream &F)
        : CompilerInstance(CI)
        , LangOptions(CI.getLangOpts())
        , PrintingPolicy(LangOptions)
        , SourceManager(CI.getSourceManager())
        , MangleContext(CI.getASTContext().createMangleContext())
        , LlvmFactsDiagnosticConsumer(F, CI.getDiagnosticOpts(),
                                      false) // doesn't own stream
        , LlvmFactsFile(F) {
        DiagnosticsEngine &DE = CI.getDiagnostics();
        NormalDiagnosticConsumer = DE.takeClient();
        LlvmFactsDiagnosticConsumer.BeginSourceFile(CI.getLangOpts(),
                                                    &CI.getPreprocessor());
      }

      ~CompilationInfo();

      const CompilerInstance &CompilerInstance;
      const LangOptions &LangOptions;
      const PrintingPolicy PrintingPolicy;
      const SourceManager &SourceManager;
      MangleContext *MangleContext;
      TextDiagnosticPrinter LlvmFactsDiagnosticConsumer;
      DiagnosticConsumer *NormalDiagnosticConsumer;
      const llvm::raw_ostream &LlvmFactsFile;
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

    // inline DiagnosticConsumer& CompilationInfo::getLlvmFactsDiagnosticConsumer() {
    //   return LlvmFactsDiagnosticConsumer;
    // }

    inline void CompilationInfo::setNormalDiagnosticConsumer() {
      DiagnosticsEngine &DE = CompilerInstance.getDiagnostics();
      if (DE.getClient() != NormalDiagnosticConsumer)
        DE.setClient(NormalDiagnosticConsumer, false); // doesn't own client
    }

    inline void CompilationInfo::setLlvmFactsDiagnosticConsumer() {
      DiagnosticsEngine &DE = CompilerInstance.getDiagnostics();
      if (DE.getClient() != &LlvmFactsDiagnosticConsumer)
        DE.setClient(&LlvmFactsDiagnosticConsumer, false); // doesn't own client
    }

  } // End namespace crisp::prolog

} // End namespace crisp

#endif
