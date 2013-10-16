# LLVM tools are usually compiled with the following options
add_definitions(-fno-rtti)
add_definitions(-fno-exceptions)

add_subdirectory(Support)
add_subdirectory(DeclExtractor)
add_subdirectory(PrologEngineWrapper)

# The order is relevant!!! The next two subdirectories use variable
# DECLEXTRACTOR_BUILD_DIR, set in subdirectory DeclExtractor.
add_subdirectory(CrispClangPlugin)
add_subdirectory(CrispLLVMPass)
