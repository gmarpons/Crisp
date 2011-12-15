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

#include "clang/Frontend/CompilerInstance.h"

using namespace clang;

namespace prolog {

  struct CompilationInfo {
    CompilationInfo(CompilerInstance &CI) : CompilerInstance(CI) {}
    CompilerInstance &CompilerInstance;
  };

  CompilationInfo* getCompilationInfo();

  void newCompilationInfo(CompilerInstance &CI);

  void deleteCompilationInfo();
    

} // End of namespace prolog

#endif
