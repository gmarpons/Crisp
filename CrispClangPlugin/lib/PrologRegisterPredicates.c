#include "PrologPredicates.h"
#include "PrologRegisterPredicates.h"

void plRegisterPredicates() {
  PL_register_foreign("name", 2, (pl_function_t) &pl_name, 0);
}
