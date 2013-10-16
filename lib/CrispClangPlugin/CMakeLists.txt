add_library(run_clang_decl_extractor STATIC
    ClangDeclarations.cpp)

add_dependencies(run_clang_decl_extractor declextractor)
add_dependencies(run_clang_decl_extractor generate_saved_state)

set_source_files_properties(ClangDeclarations.cpp PROPERTIES
    COMPILE_FLAGS "-Xclang -load -Xclang ${DECLEXTRACTOR_BUILD_DIR}/libdeclextractor.so -Xclang -add-plugin -Xclang decl-extractor -Xclang -plugin-arg-decl-extractor -Xclang ${CMAKE_BINARY_DIR}/${SWI_BOOT_FILES_DIR} -Xclang -plugin-arg-decl-extractor -Xclang init_clang_base_types")

add_library(crispclang SHARED
    ClangPrologPredicateRegistration.c
    ClangPrologPredicates.cpp
    CompilationInfo.cpp
    CrispASTAction.cpp)

target_link_libraries(crispclang
  PrologEngineWrapper
  Support
  ${SWIPL_LIBRARIES})

add_dependencies(crispclang run_clang_decl_extractor)

include_directories(${CRISP_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${SWIPL_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(${LLVM_DEFINITIONS})
add_definitions(${CLANG_DEFINITIONS})
add_definitions(${SWIPL_DEFINITIONS})
add_definitions(
  -DSWI_BOOT_FILES_DIR="${SWI_BOOT_FILES_DIR}"
  -DPROLOG_RULES_DIR="${PROLOG_RULES_DIR}")

# TODO: add all necessary definitions to avoid using config.h.

install(TARGETS crispclang
    LIBRARY DESTINATION lib)
