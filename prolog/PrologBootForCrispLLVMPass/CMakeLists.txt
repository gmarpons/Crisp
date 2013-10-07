set(SWI_BOOT_FILE "${BINARY_SWI_BOOT_FILES_DIR}/PrologBootForCrispLLVMPass.sh")

add_swi_prolog_saved_state("${SWI_BOOT_FILE}" PrologBootForCrispLLVMPass.pl)
install(FILES ${SWI_BOOT_FILE} DESTINATION "${INSTALL_SWI_BOOT_FILES_DIR}")
