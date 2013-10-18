add_library(run_llvm_decl_extractor STATIC
    LLVMDeclarations.cpp)

add_dependencies(run_llvm_decl_extractor declextractor)
add_dependencies(run_llvm_decl_extractor generate_saved_state)

set_source_files_properties(LLVMDeclarations.cpp PROPERTIES
    COMPILE_FLAGS "-Xclang -load -Xclang ${DECLEXTRACTOR_BUILD_DIR}/libdeclextractor.so -Xclang -add-plugin -Xclang decl-extractor -Xclang -plugin-arg-decl-extractor -Xclang ${CMAKE_BINARY_DIR}/${SWI_BOOT_FILES_DIR} -Xclang -plugin-arg-decl-extractor -Xclang init_llvm_base_types")

add_library(crispllvm SHARED
    LLVMCompilationInfo.cpp
    LLVMPrologPredicateRegistration.c
    LLVMPrologPredicates.cpp
    LLVMPrologQueries.cpp
    CrispModulePass.cpp)

target_link_libraries(crispllvm
  PrologEngineWrapper
  Support
  ${SWIPL_LIBRARIES})

add_dependencies(crispllvm run_llvm_decl_extractor)

include_directories(${CRISP_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${SWIPL_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(${LLVM_DEFINITIONS})
add_definitions(${SWIPL_DEFINITIONS})
add_definitions(
  -DSWI_BOOT_FILES_DIR="${SWI_BOOT_FILES_DIR}"
  -DPROLOG_RULES_DIR="${PROLOG_RULES_DIR}")

install(TARGETS crispllvm LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
