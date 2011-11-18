#include <cstdlib>
#include <cstring>

#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "PrologRegisterPredicates.h"
#include "PrologEngine.h"

using namespace llvm;
using namespace clang;

namespace prolog {

  static char *RulesFileNameCStr;

  int plRunEngine(const std::string &RulesFileName) {
    plRegisterPredicates();

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

    int Success = PL_initialise(sizeof(argv) / sizeof(argv[0]), argv);
    DEBUG(if ( !Success)
            dbgs() << "Could not initialize Prolog engine.\n");
    return Success;
  }

  int plCleanUp(int Status) {
    int Success = PL_cleanup(Status);
    free(RulesFileNameCStr);
    DEBUG(if ( !Success) dbgs() << "Prolog engine clean up failed.\n");
    return Success;
  }

  int plInteractiveSession() {
    fid_t FId = PL_open_foreign_frame();
    functor_t WelcomeF = PL_new_functor(PL_new_atom("welcome_msg"), 0);
    term_t WelcomeT = PL_new_term_ref();
    if ( !PL_cons_functor(WelcomeT, WelcomeF)) {
      PL_discard_foreign_frame(FId);
      return FALSE;
    }
    if ( !PL_call(WelcomeT, NULL))  {
      PL_discard_foreign_frame(FId);
      return FALSE;
    }
    functor_t PrologF = PL_new_functor(PL_new_atom("prolog"), 0);
    term_t PrologT = PL_new_term_ref();
    if ( !PL_cons_functor(PrologT, PrologF))  {
      PL_discard_foreign_frame(FId);
      return FALSE;
    }
    int Success = PL_call(PrologT, NULL);
    PL_discard_foreign_frame(FId);
    DEBUG(if ( !Success) 
            dbgs() << "Could not start Prolog interactive session.\n");
    return Success;
  }

  int plAssertDeclIsA(const Decl *Decl, const std::string &Sort) {
    int Success;
    term_t DeclT = PL_new_term_ref();
    Success = PL_put_pointer(DeclT, (void *) Decl);
    if ( !Success) return Success;
    term_t SortA = PL_new_term_ref();
    PL_put_atom_chars(SortA, Sort.c_str());
    functor_t IsAF = PL_new_functor(PL_new_atom("isA"), 2);
    term_t IsAT = PL_new_term_ref();
    Success = PL_cons_functor(IsAT, IsAF, DeclT, SortA);
    if ( !Success) return Success;
    functor_t AssertzF = PL_new_functor(PL_new_atom("assertz"), 1);
    term_t AssertzT = PL_new_term_ref();
    Success = PL_cons_functor(AssertzT, AssertzF, IsAT);
    if ( !Success) return Success;
    Success = PL_call(AssertzT, NULL);
    DEBUG(if ( !Success) dbgs() << "Error asserting 'isA " 
                                << Sort << "' fact.\n");
    return Success;
  }

}
