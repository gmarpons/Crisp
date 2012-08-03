
add_subdirectory(DeclExtractor)
add_subdirectory(PrologEngineWrapper)
# the order is important!!!
# these two are using variable from DeclExtractor CMake file
add_subdirectory(CrispClangPlugin)
add_subdirectory(CrispLLVMPass)
