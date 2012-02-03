// RunPrologEngine.cpp -----------------------------------------------*- C++ -*-

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

#include <crisp/Config/config.h>

#include <cstdlib>
#include <cstring>
#include <SWI-Prolog.h>

#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"

#include "crisp/RunPrologEngine.h"

// Stringify environment variables
#define XSTR(s) STR(s)
#define STR(s) #s

namespace fs = llvm::sys::fs;
using namespace llvm;

namespace prolog {

  static char *BootFileAbsNameCStr;

  int plRunEngine(const std::string &BootFileName) {
    // Look for dir where saved state files for booting Prolog are
    // placed: either build data dir or install data dir, depending on
    // whether DATA_OBJ_ROOT is defined (in order to define it, its
    // necessary to set ENABLE_DATA_OBJ_ROOT variable when compiling).
#ifdef DATA_OBJ_ROOT
    fs::directory_entry BootFilesDir(XSTR(DATA_OBJ_ROOT));
#else
    fs::directory_entry BootFilesDir(XSTR(DATA_INSTALL_ROOT));
#endif  // ifdef DATA_OBJ_ROOT

    std::string S(BootFilesDir.path());
    S.append("/");
    S.append(BootFileName);
    BootFileAbsNameCStr = (char *) malloc(sizeof(char) * (S.size() + 1));
    strcpy(BootFileAbsNameCStr, S.c_str());

    // Necessary to tell SWI where is its Home directory, to find
    // initialization files
    putenv((char *) "SWI_HOME_DIR=" XSTR(SWIPL_BASE_DIR));

    // Set arguments for call to Prolog. First argument is the
    // executable file name. We don't set this as it's difficult to do
    // it in a portable manner. SWI uses this file for 2 purposes:
    // 1. See whether a Prolog saved-state is appended to the file.
    // 2. Find the Prolog home directory.
    // We don't need any of them.
    char *argv[] = {
      (char *) "oh-my"          // Stub name (see previous comment)
      , (char *) "-x"           // Boot script
      , BootFileAbsNameCStr
      , (char *) "-g"           // Goal executed before top level
      , (char *) "init_msg"
      , (char *) "-t"           // Goal used as interactive top level
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
    free(BootFileAbsNameCStr);
    DEBUG(if ( !Success) dbgs() << "Prolog engine clean up failed.\n");
    return Success;
  }

  int plInteractiveSession() {
    return PL_toplevel();
  }

}
