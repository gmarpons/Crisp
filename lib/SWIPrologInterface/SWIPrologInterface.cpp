// SWIPrologInterface.cpp: Embbedding a SWI Prolog engine. -*- C++ -*-

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

#include <cstdlib>
#include <cstring>
#include <SWI-Prolog.h>

#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "SWIPrologInterface.h"

using namespace llvm;

namespace prolog {

  static char *ScriptFileNameCStr;

  int plRunEngine(const std::string &ScriptFileName) {
    // FIXME: following dir as a configuration option.
    putenv((char *) "SWIPL=/usr/lib/swi-prolog");
    ScriptFileNameCStr
      = (char *) malloc(sizeof(char) * (ScriptFileName.size() + 1));
    strcpy(ScriptFileNameCStr, ScriptFileName.c_str());
    char *argv[] = {
      (char *) "oh-my"          // FIXME: how to find the name of the
                                // executable from inside the plugin?
      // , (char *) "-q"           // Operate quietly
      , (char *) "-s"           // Load script
      , ScriptFileNameCStr
      , (char *) "-g"           // Goal executed before entering top
                                // level
      , (char *) "init_msg"
      , (char *) "-t"           // Goal used as an interactive top
                                // level
      , (char *) "welcome_msg_and_prolog"
    };

    int Success = PL_initialise(sizeof(argv) / sizeof(argv[0]), argv);
    if (Success) PL_install_readline();

    DEBUG(if ( !Success)
            dbgs() << "Could not initialize Prolog engine.\n");
    return Success;
  }

  int plCleanUp(int Status) {
    int Success = PL_cleanup(Status);
    free(ScriptFileNameCStr);
    DEBUG(if ( !Success) dbgs() << "Prolog engine clean up failed.\n");
    return Success;
  }

  int plInteractiveSession() {
    return PL_toplevel();
  }

}
