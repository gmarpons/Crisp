#include <SWI-Prolog.h>

#ifdef __cplusplus
extern "C" {
#endif

  foreign_t pl_declName(term_t DeclT, term_t NameT);
  foreign_t pl_typeName(term_t TypeT, term_t NameT);
  foreign_t pl_getPointeeType(term_t TypeT, term_t PointeeT);
  foreign_t pl_getCanonicalTypeUnqualified(term_t TypeT, term_t DesugaredT);
  foreign_t pl_getResultType(term_t TypeT, term_t ResultT);
  foreign_t pl_getType(term_t ValueDeclT, term_t TypeT);

#ifdef __cplusplus
}
#endif
