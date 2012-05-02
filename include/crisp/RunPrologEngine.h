// include/crisp/RunPrologEngine.h -----------------------------------*- C++ -*-

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
/// \brief An isolation layer between Crisp and the SWI Prolog C interface.
///
/// Functions necessary to run and clean up a Prolog Engine, and start
/// an interactive session.

#ifndef RUNPROLOGENGINE_H
#define RUNPROLOGENGINE_H

#include <string>

namespace crisp {

  /// Interface with Prolog Engine.
  namespace prolog {

    int plRunEngine(const std::string& BootFileName,
                    const std::string& InitialGoal = std::string("init_msg"));
    int plCleanUp(int Status);
    int plInteractiveSession();

  } // End namespace crisp::prolog

} // End namespace crisp

#endif
