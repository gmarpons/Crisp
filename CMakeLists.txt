
project(Crisp)
cmake_minimum_required(VERSION 2.8)

# Search for modules here, before checking default CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Requisites
find_package(Threads REQUIRED)
find_package(Boost 1.46.1 REQUIRED)
find_package(Swipl REQUIRED)
find_package(Clang REQUIRED)

list(APPEND include_dirs ${CMAKE_CURRENT_SOURCE_DIR}/include)
list(APPEND include_dirs ${CMAKE_CURRENT_BINARY_DIR}/include)
set(CRISP_INCLUDE_DIRS ${include_dirs})

set(CMAKE_CXX_COMPILER "${CLANG_EXECUTABLE}++")
set(CMAKE_C_COMPILER "${CLANG_EXECUTABLE}")

add_definitions("-DDATA_INSTALL_ROOT=${CMAKE_INSTALL_PREFIX}/share")

add_subdirectory(include)
add_subdirectory(prolog)
# order is important!!! lib is using variable from prolog.
add_subdirectory(lib)
add_subdirectory(test)
