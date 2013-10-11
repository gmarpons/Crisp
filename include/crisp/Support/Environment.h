// include/crisp/Support/Environment.h -------------------------------*- C++ -*-

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

/// \file
/// \brief General utilities about the execution environment.

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <string>

namespace crisp {

  /// Takes a relative path and returns an absolute path. The prefix path used for the
  /// absolute path depends on the environment variable `CRISPTEST'.
  /// \param RelPath A relative path.
  /// \return If `CRISPTEST' has value "1", the path returned is
  /// "CMAKE_BINARY_DIR/RelPath". Otherwise, it returns
  /// "CMAKE_INSTALL_FULL_DATADIR/PROJECT_NAME/RelPath", where all capitalized variable
  /// names are defined at build time.
  const std::string prefixWithBuildOrInstallFullPath(const std::string& RelPath);

} // End namespace crisp

#endif










