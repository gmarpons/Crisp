// LLVMCompilationInfo.cpp -------------------------------------------*- C++ -*-

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

#include "LLVMCompilationInfo.h"

namespace crisp {

  namespace prolog {

    static LLVMCompilationInfo *LLVMCompilationInfoSingleton;

    LLVMCompilationInfo* getLLVMCompilationInfo() {
      return LLVMCompilationInfoSingleton;
    }

    void newLLVMCompilationInfo(Pass &P, Module &M) {
      if (LLVMCompilationInfoSingleton) delete LLVMCompilationInfoSingleton;
      LLVMCompilationInfoSingleton = new LLVMCompilationInfo(P, M);
    }

    void deleteLLVMCompilationInfo() {
      if (LLVMCompilationInfoSingleton) delete LLVMCompilationInfoSingleton;
    }

    LLVMCompilationInfo::~LLVMCompilationInfo() {
      // No dynamic memory to free.
    }

  } // End namespace crisp::prolog

} // End namespace crisp
