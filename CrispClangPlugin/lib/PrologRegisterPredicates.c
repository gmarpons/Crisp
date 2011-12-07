#include "PrologPredicates.h"
#include "PrologRegisterPredicates.h"

void plRegisterPredicates() {
  PL_register_foreign("declName", 2, (pl_function_t) &pl_declName, 0);
  PL_register_foreign("typeName", 2, (pl_function_t) &pl_typeName, 0);
  PL_register_foreign("getPointeeType", 2
                      , (pl_function_t) &pl_getPointeeType, 0);
  PL_register_foreign("getCanonicalTypeUnqualified", 2
                      , (pl_function_t) &pl_getCanonicalTypeUnqualified, 0);
  PL_register_foreign("getResultType", 2
                      , (pl_function_t) &pl_getResultType, 0);
  PL_register_foreign("getType", 2
                      , (pl_function_t) &pl_getType, 0);
}
