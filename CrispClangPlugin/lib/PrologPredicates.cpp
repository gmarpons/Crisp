#include "clang/AST/Decl.h"

#include "PrologPredicates.h"

using namespace clang;

foreign_t pl_name(term_t DC, term_t Name) {
  Decl *D;
  if (!PL_get_pointer(DC, (void **) &D))
    return PL_warning("name/2: instantiation fault");
  if (NamedDecl *ND = dyn_cast<NamedDecl>(D)) {
    return PL_unify_atom_chars(Name, ND->getNameAsString().c_str());
  }
  return TRUE;
}
