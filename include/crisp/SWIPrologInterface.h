// include/crisp/SWIPrologInterface.h --------------------------------*- C++ -*-

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
/// An isolation layer between Crisp and the SWI Prolog C interface.

#ifndef SWIPROLOGINTERFACE_H
#define SWIPROLOGINTERFACE_H

#include <string>

/// Interface with Prolog Engine.
namespace prolog {
  int plRunEngine(const std::string &BootFileName);
  int plCleanUp(int Status);
  int plInteractiveSession();
} // End prolog namespace

#endif
