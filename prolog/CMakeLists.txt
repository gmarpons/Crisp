file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${SWI_BOOT_FILES_DIR}")

add_subdirectory(PrologBootForDeclExtractor)
add_subdirectory(PrologBootForCrispClangPlugin)
add_subdirectory(PrologBootForCrispLLVMPass)
add_subdirectory(Rules)
