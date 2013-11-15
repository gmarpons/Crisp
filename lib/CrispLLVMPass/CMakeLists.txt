# This library target is not meant to be linked anywhere. Its build process generates a
# .inc file used by targed 'crispclang'.
add_library(run_llvm_decl_extractor STATIC
  LLVMDeclarations.cpp)

add_dependencies(run_llvm_decl_extractor declextractor)

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

# prolog_boot_for_crisp_llvm_pass is not really needed for crispllvm, but all clients of
# crispllvm are going to need it, so we set a dependency.
add_dependencies(crispllvm prolog_boot_for_crisp_llvm_pass)
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
