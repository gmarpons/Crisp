#include "clang/AST/Decl.h"

#include "PrologPredicates.h"

using namespace clang;

foreign_t pl_declName(term_t DeclT, term_t NameT) {
  Decl *D;
  if (!PL_get_pointer(DeclT, (void **) &D))
    return PL_warning("declName/2: instantiation fault");
  if (NamedDecl *ND = dyn_cast<NamedDecl>(D)) {
    return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
  }
  return FALSE;
}

foreign_t pl_typeName(term_t TypeT, term_t NameT) {
  Type *T;
  if (!PL_get_pointer(TypeT, (void **) &T))
    return PL_warning("typeName/2: instantiation fault");
  if (RecordType *RT = dyn_cast<RecordType>(T)) {
    NamedDecl *ND = RT->getDecl();
    return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
  }
  else if (TypedefType *TT = dyn_cast<TypedefType>(T)) {
    NamedDecl *ND = TT->getDecl();
    return PL_unify_atom_chars(NameT, ND->getNameAsString().c_str());
  }
  // TODO: add all the other Type subclasses that have a getDecl() method
  // TODO: add a special case for BuiltinType
  return FALSE;
}

foreign_t pl_getPointeeType(term_t TypeT, term_t PointeeT) {
  Type *T;
  if (!PL_get_pointer(TypeT, (void **) &T))
    return PL_warning("getPointeeType/2: instantiation fault");
  if (PointerType *PT = dyn_cast<PointerType>(T)) {
    const Type *PointeeAux = PT->getPointeeType().getTypePtr();
    int Success;
    term_t PointeeAuxT = PL_new_term_ref();
    Success = PL_put_pointer(PointeeAuxT, (void *) PointeeAux);
    return PL_unify(PointeeT, PointeeAuxT);
  }
  return FALSE;
}

foreign_t pl_getCanonicalTypeUnqualified(term_t TypeT, term_t DesugaredT) {
  Type *T;
  if (!PL_get_pointer(TypeT, (void **) &T))
    return PL_warning("getUnqualifiedDesugaredType/2: instantiation fault");
  const Type *DesugaredAux = T->getCanonicalTypeUnqualified().getTypePtr();
  int Success;
  term_t DesugaredAuxT = PL_new_term_ref();
  Success = PL_put_pointer(DesugaredAuxT, (void *) DesugaredAux);
  return PL_unify(DesugaredT, DesugaredAuxT);
}
