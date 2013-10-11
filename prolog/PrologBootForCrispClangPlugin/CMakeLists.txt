set(SWI_BOOT_FILE
  "${CMAKE_BINARY_DIR}/${SWI_BOOT_FILES_DIR}/PrologBootForCrispClangPlugin.sh")
add_swi_prolog_saved_state("${SWI_BOOT_FILE}" PrologBootForCrispClangPlugin.pl)
install(FILES ${SWI_BOOT_FILE} 
  DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}/${SWI_BOOT_FILES_DIR}")
