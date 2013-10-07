set(SWI_BOOT_FILE "${BINARY_SWI_BOOT_FILES_DIR}/PrologBootForDeclExtractor.sh")

add_swi_prolog_saved_state("${SWI_BOOT_FILE}" PrologBootForDeclExtractor.pl)
