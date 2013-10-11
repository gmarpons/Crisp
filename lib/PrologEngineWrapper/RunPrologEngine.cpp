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

namespace crisp {

  namespace prolog {

    static char *BootFileAbsNameCStr;

    int plRunEngine(const std::string& BootFileName,
                    const std::string& BootFilesDir,
                    const std::string& InitialGoalFunctor) {

      std::string S(BootFilesDir);
      S.append("/");
      S.append(BootFileName);
      BootFileAbsNameCStr = (char *) malloc(sizeof(char) * (S.size() + 1));
      strcpy(BootFileAbsNameCStr, S.c_str());
      std::string DebugFlag("false");
      DEBUG(DebugFlag = "true");
      std::string InitialGoal = InitialGoalFunctor + "(" + DebugFlag + ")";
      // Twine InitialGoal = Twine(InitialGoalFunctor) + Twine("(")
      //   + Twine(DebugFlag) + Twine(")");

      // TODO: use --home option instead of env. var: read
      // http://www.swi-prolog.org/pldoc/man?section=findhome
      // Maybe not necessary to give home at all, if using a
      // self-contained saved state.

      // Necessary to tell SWI where is its Home directory, to find
      // initialization files
      putenv((char *) "SWI_HOME_DIR=" XSTR(SWIPL_ROOT_DIR));

      // Set arguments for call to Prolog. First argument is the
      // executable file name. We don't set this as it's difficult to do
      // it in a portable manner. SWI uses this file for 2 purposes:
      // 1. See whether a Prolog saved-state is appended to the file.
      // 2. Find the Prolog home directory.
      // We don't need any of them.
      char *argv[] = {          // Constness is lost in every element
        (char *) "oh-my"        // Stub name (see previous comment)
        , (char *) "-x"         // Boot script
        , BootFileAbsNameCStr
        , (char *) "-g"         // Goal executed before top level
        , (char *) InitialGoal.c_str()
        , (char *) "-t"         // Goal used as interactive top level
        , (char *) "welcome_msg_and_prolog"
      };

      int Success = PL_initialise(sizeof(argv) / sizeof(argv[0]), argv);
      if (Success) PL_install_readline();

      DEBUG(if ( !Success)
              dbgs() << "Could not initialize Prolog engine." << "\n");
      return Success;
    }

    int plCleanUp(int Status) {
      int Success = PL_cleanup(Status);
      free(BootFileAbsNameCStr);
      DEBUG(if ( !Success) dbgs() << "Prolog engine clean up failed." << "\n");
      return Success;
    }

    int plInteractiveSession() {
      return PL_toplevel();
    }

    int plLoadFile(const std::string& FileBaseName,
                   const std::string& RulesDir) {
      int Success;
      term_t FileBaseNameA = PL_new_term_ref();
      Success = PL_put_atom_chars(FileBaseNameA, FileBaseName.c_str());
      if ( !Success) return Success;
      term_t RulesDirA = PL_new_term_ref();
      Success = PL_put_atom_chars(RulesDirA, RulesDir.c_str());
      if ( !Success) return Success;
      functor_t LoadFileF = PL_new_functor(PL_new_atom("load_file"), 2);
      term_t LoadFileT = PL_new_term_ref();
      Success = PL_cons_functor(LoadFileT, LoadFileF, FileBaseNameA, RulesDirA);
      if ( !Success) return Success;
      Success = PL_call(LoadFileT, NULL);
      DEBUG(if ( !Success) dbgs() << "Error executing goal 'load_file("
                                  << FileBaseName << ")'." << "\n");
      return Success;
    }

  } // End namespace crisp::prolog

} // End namespace crisp
