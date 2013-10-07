file(MAKE_DIRECTORY ${BINARY_SWI_BOOT_FILES_DIR})

add_subdirectory(PrologBootForDeclExtractor)
add_subdirectory(PrologBootForCrispClangPlugin)
add_subdirectory(PrologBootForCrispLLVMPass)
add_subdirectory(Rules)

# TODO: install

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/PrologBootForCrispClangPlugin.sh
    ${CMAKE_CURRENT_BINARY_DIR}/PrologBootForCrispLLVMPass.sh
  DESTINATION share)

install(
  DIRECTORY Rules/
  DESTINATION share
  FILES_MATCHING PATTERN "*.pl")
