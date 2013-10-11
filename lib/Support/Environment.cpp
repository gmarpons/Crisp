// Environment.cpp ---------------------------------------------------*- C++ -*-

// Copyright (C) 2011-2013 Guillem Marpons <gmarpons@babel.ls.fi.upm.es>
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

#include <cstdlib>
#include <cstring>

#include "crisp/Config/config.h"
#include "crisp/Support/Environment.h"

namespace crisp {

  const std::string prefixWithBuildOrInstallFullPath(const std::string& RelPath) {
    char *CrispTestEnvVar = std::getenv("CRISPTEST");
    // If environment var "CRISPTEST" == 1 use object (build) tree files
    if (CrispTestEnvVar && !std::strcmp(CrispTestEnvVar, "1")) {
      return std::string(CMAKE_BINARY_DIR "/") + RelPath;
    } else {                    // else use install tree files
      return std::string(CMAKE_INSTALL_FULL_DATADIR "/" PROJECT_NAME "/") + RelPath;
    }
  }

} // End namespace crisp











