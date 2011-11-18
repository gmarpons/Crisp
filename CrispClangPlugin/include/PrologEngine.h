#include <SWI-Prolog.h>
#include <string>

#include "clang/AST/Decl.h"

using namespace clang;

namespace prolog {
  int plRunEngine(const std::string &RulesFileName);
  int plCleanUp(int Status);
  int plInteractiveSession();
  int plAssertDeclIsA(const Decl *Decl, const std::string &Sort);
}
