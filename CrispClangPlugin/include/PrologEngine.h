#include <SWI-Prolog.h>
#include <string>

namespace prolog {
  void plRunEngine(std::string &RulesFileName);
  void plCleanUp();
  void plTopLevel();
}
