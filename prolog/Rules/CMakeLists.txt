# All testing rule files appended to ${RULE_FILES}.
list(APPEND RULE_FILES SomeHICPPrules.pl)

# Do copy all rule files to ${BINARY_PROLOG_RULES_DIR} for testing.
foreach(rule_file ${RULE_FILES})
  configure_file(${rule_file} "${BINARY_PROLOG_RULES_DIR}/${rule_file}" COPYONLY)
endforeach()
