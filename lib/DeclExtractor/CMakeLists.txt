set(DECLEXTRACTOR_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

add_library(declextractor SHARED
    DeclExtractorASTAction.cpp
    DeclExtractorPrologPredicateRegistration.c
    DeclExtractorPrologPredicates.cpp)

include_directories(${CRISP_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${SWIPL_INCLUDE_DIRS})

add_definitions(${LLVM_DEFINITIONS})
add_definitions(${CLANG_DEFINITIONS})
add_definitions(${SWIPL_DEFINITIONS})

target_link_libraries(declextractor PrologEngineWrapper ${SWIPL_LIBRARIES})
