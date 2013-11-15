set(SWI_BOOT_FILE
  "${CMAKE_BINARY_DIR}/${SWI_BOOT_FILES_DIR}/PrologBootForCrispLLVMPass.sh")
add_swi_prolog_saved_state("${SWI_BOOT_FILE}"
  prolog_boot_for_crisp_llvm_pass
  PrologBootForCrispLLVMPass.pl)

install(FILES ${SWI_BOOT_FILE}
  DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}/${SWI_BOOT_FILES_DIR}")
