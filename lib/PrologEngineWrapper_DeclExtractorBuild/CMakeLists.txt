
add_library(PrologEngineWrapper_DeclExtractorBuild STATIC
    ../PrologEngineWrapper/ClangPrologQueries.cpp
    ../PrologEngineWrapper/PrologUtilityFunctions.cpp
    ../PrologEngineWrapper/RunPrologEngine.cpp)

include_directories(${CRISP_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${SWIPL_INCLUDE_DIRS})

add_definitions(${CLANG_DEFINITIONS})
add_definitions(${SWIPL_DEFINITIONS})
add_definitions("-DDATA_OBJ_ROOT=${DATA_OBJ_ROOT}")
