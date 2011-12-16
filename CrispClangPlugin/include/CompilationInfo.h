// CompilationInfo.h: Global data to be used by Prolog predicates. -*- C++ -*-

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

#ifndef CRISPCLANGPLUGIN_COMPILATIONINFO_H
#define CRISPCLANGPLUGIN_COMPILATIONINFO_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/Mangle.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/Frontend/CompilerInstance.h"

using namespace clang;

namespace prolog {

  class CompilationInfo {
  public:
    const CompilerInstance& getCompilerInstance() const;
    
    const LangOptions& getLangOpts() const;
    
    const PrintingPolicy& getPrintingPolicy() const;

    const SourceManager& getSourceManager() const;

    MangleContext* getMangleContext();

    friend void newCompilationInfo(CompilerInstance &CI);
    friend void deleteCompilationInfo();

  private:
    CompilationInfo(CompilerInstance &CI)
      : CompilerInstance(CI)
      , LangOptions(CI.getLangOpts())
      , PrintingPolicy(LangOptions)
      , SourceManager(CI.getSourceManager())
      , MangleContext(CI.getASTContext().createMangleContext()) {
    }

    ~CompilationInfo();

    const CompilerInstance &CompilerInstance;
    const LangOptions &LangOptions;
    const PrintingPolicy PrintingPolicy;
    const SourceManager &SourceManager;
    MangleContext *MangleContext;
  };

  CompilationInfo* getCompilationInfo();

  void newCompilationInfo(CompilerInstance &CI);

  void deleteCompilationInfo();

  inline const CompilerInstance& CompilationInfo::getCompilerInstance() const {
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

} // End of namespace prolog

#endif
