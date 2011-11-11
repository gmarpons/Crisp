#include "PrologEngine.h"
#include <cstdlib>
#include <cstring>

namespace prolog {

  static char *RulesFileNameCStr;

  void plRunEngine(std::string &RulesFileName) {
    // FIXME: following dir as a configuration option.
    putenv((char *) "SWIPL=/usr/lib/swi-prolog");
    RulesFileNameCStr
      = (char *) malloc(sizeof(char) * (RulesFileName.size() + 1));
    strcpy(RulesFileNameCStr, RulesFileName.c_str());
    char *argv[] = {
      (char *) "oh-my"          // FIXME: how to find the name of the
                                // executable from inside the plugin?
      // , (char *) "-q"           // Operate quietly
      , (char *) "-s"           // Load script
      , RulesFileNameCStr
      , (char *) "-g"           // Goal executed before entering top
                                // level
      , (char *) "init_msg"
      , (char *) "-t"           // Goal used as an interactive top
                                // level
      , (char *) "halt"
    };

    if ( !PL_initialise(sizeof(argv) / sizeof(argv[0]), argv) ) {
      PL_cleanup(1);
      return;
    }
  }

  void plCleanUp() {
    PL_cleanup(0);
    free(RulesFileNameCStr);
  }

  void plTopLevel() {
    functor_t WelcomeF = PL_new_functor(PL_new_atom("welcome_msg"), 0);
    term_t WelcomeT = PL_new_term_ref();
    PL_cons_functor(WelcomeT, WelcomeF); // TODO: handle possible error
    PL_call(WelcomeT, NULL);
    functor_t PrologF = PL_new_functor(PL_new_atom("prolog"), 0);
    term_t PrologT = PL_new_term_ref();
    PL_cons_functor(PrologT, PrologF); // TODO: handle possible error
    PL_call(PrologT, NULL);
  }

}
