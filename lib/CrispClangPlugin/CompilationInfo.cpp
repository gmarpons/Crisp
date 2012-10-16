// CompilationInfo.cpp -----------------------------------------------*- C++ -*-

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

#include "llvm/ADT/Twine.h"
#include "llvm/Support/Debug.h"

#include "CompilationInfo.h"

using namespace clang;

namespace crisp {

  namespace prolog {

    static CompilationInfo *CompilationInfoSingleton;

    CompilationInfo* getCompilationInfo() {
      return CompilationInfoSingleton;
    }

    void newCompilationInfo(CompilerInstance &CI) {
      if (CompilationInfoSingleton) delete CompilationInfoSingleton;

      // Get main file name
      SourceManager& SM(CI.getASTContext().getSourceManager());
      FileID MainFileID = SM.getMainFileID();
      const char* MainFileName = SM.getFileEntryForID(MainFileID)->getName();
      DEBUG(llvm::dbgs() << "Main source file name: " << MainFileName << "\n");

      // Open file for facts for LLVM
      Twine LlvmFactsFileName = MainFileName + Twine(".diags");
      std::string ErrorInfo;

      // The following stream is created and never destroyed
      static llvm::raw_fd_ostream
        LlvmFactsFile(LlvmFactsFileName.str().c_str(), ErrorInfo);
      if ( !ErrorInfo.empty()) {
        llvm::errs() <<"Fatal error opening file " << LlvmFactsFileName << "\n";
        abort();                // TODO: quit gracefully
      }

      // Create singleton
      CompilationInfoSingleton = new CompilationInfo(CI, LlvmFactsFile);
    }

    void deleteCompilationInfo() {
      if (CompilationInfoSingleton) delete CompilationInfoSingleton;
    }

    CompilationInfo::~CompilationInfo() {
      DiagnosticsEngine &DE = CompilerInstance.getDiagnostics();
      DE.setClient(NormalDiagnosticConsumer, true); // owns client
      LlvmFactsDiagnosticConsumer.EndSourceFile();
      delete MangleContext;
    }

  } // End namespace crisp::prolog

} // End namespace crisp
