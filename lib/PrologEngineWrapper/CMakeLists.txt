
add_library(PrologEngineWrapper STATIC
    ClangPrologQueries.cpp
    PrologUtilityFunctions.cpp
    RunPrologEngine.cpp)

include_directories(${CRISP_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${SWIPL_INCLUDE_DIRS})

add_definitions(${CLANG_DEFINITIONS})
add_definitions(${SWIPL_DEFINITIONS})
