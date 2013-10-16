# Project name
project(Crisp C CXX)

# Required CMake version and policies
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

# Declare Crisp version number
set(Crisp_VERSION_MAJOR 0)
set(Crisp_VERSION_MINOR 2)
set(Crisp_VERSION_PATCH 0)

# Search for modules in source tree, before checking default CMake
# modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Requisites
find_package(Threads          REQUIRED)
find_package(Boost    1.46.1  REQUIRED)
find_package(Swipl    6.2     REQUIRED)

include(llvm-clang)
find_llvm_and_clang(3.2 3.2)

# Debug messages about requisites
message(STATUS "DEBUG LLVM-CONFIG_EXECUTABLE: ${LLVM-CONFIG_EXECUTABLE}")
message(STATUS "DEBUG LLVM_TOOLS_BINARY_DIR: ${LLVM_TOOLS_BINARY_DIR}")
message(STATUS "DEBUG LLVM_VERSION: ${LLVM_VERSION}")
message(STATUS "DEBUG CLANG_VERSION: ${CLANG_VERSION}")
message(STATUS "DEBUG SWIPL_VERSION_STRING: ${SWIPL_VERSION_STRING}")
message(STATUS "DEBUG SWIPL_ROOT_DIR: ${SWIPL_ROOT_DIR}")
message(STATUS "DEBUG CLANG_EXECUTABLE: ${CLANG_EXECUTABLE}")
message(STATUS "DEBUG LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")
message(STATUS "DEBUG CLANG_INCLUDE_DIRS: ${CLANG_INCLUDE_DIRS}")
message(STATUS "DEBUG SWIPL_INCLUDE_DIRS: ${SWIPL_INCLUDE_DIRS}")
message(STATUS "DEBUG LLVM_DEFINITIONS: ${LLVM_DEFINITIONS}")
message(STATUS "DEBUG LLVM_BUILD_TYPE: ${LLVM_BUILD_TYPE}")
message(STATUS "DEBUG CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# Define GNU standard directories
include(GNUInstallDirs)

# Crisp-specific include dirs
list(APPEND CRISP_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/include)
list(APPEND CRISP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Debug messages about Crisp includes
message(STATUS "DEBUG CRISP_INCLUDE_DIRS: ${CRISP_INCLUDE_DIRS}")

# TODO: move this to dirs where it's really necessary, i.e., with targets that need to
# compile with clang. This probably will solve the problem with distributions without
# clang++
set(CMAKE_CXX_COMPILER "${CLANG_EXECUTABLE}++")
set(CMAKE_C_COMPILER "${CLANG_EXECUTABLE}")

# The following variables are used in subdirectories. They represent relative paths
# meant to be prefixed with either "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}" or
# ${CMAKE_BINARY_DIR}.
set(SWI_BOOT_FILES_DIR "SWIBootFiles")
set(CRISP_RULES_DIR "CrispRules")
set(PROLOG_RULES_DIR "PrologRules")

# Install files
install(FILES COPYING README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})

# Subdirectories
add_subdirectory(include)
add_subdirectory(prolog)
add_subdirectory(lib)

# add_subdirectory(test)

# TODO: make check (only if llvm-lit present)
# TODO: make install
# TODO: make clean
