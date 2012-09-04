
add_subdirectory(DeclExtractor)
add_subdirectory(PrologEngineWrapper)
add_subdirectory(PrologEngineWrapper_DeclExtractorBuild)
# the order is important!!!
# these two are using variable from DeclExtractor CMake file
add_subdirectory(CrispClangPlugin)
add_subdirectory(CrispLLVMPass)
