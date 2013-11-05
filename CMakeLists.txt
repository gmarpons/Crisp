# Usage notes:
#
# - If Python interpreter version found by default can not be used to run regression
#   tests (version 2.7.* is needed), a warning is emitted. A different version can be
#   passed to Cmake with `-DPYTHON_EXECUTABLE=/path/to/python/interpr'.

# Project name
project(crisp C CXX)

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
find_llvm_and_clang(3.2)

find_package(PythonInterp 2.7 QUIET) # optional package
if(${PYTHON_VERSION_STRING} VERSION_GREATER 2.99.99)
  list(APPEND MSG "Needed Python 2.7.* to run regression tests, ")
  list(APPEND MSG "but version ${PYTHON_VERSION_STRING} found.")
  message(WARNING ${MSG})
else(${PYTHON_VERSION_STRING} VERSION_GREATER 2.99.99)
  message(STATUS "Python interpreter version: ${PYTHON_VERSION_STRING}")
endif(${PYTHON_VERSION_STRING} VERSION_GREATER 2.99.99)

# Define GNU standard directories
include(GNUInstallDirs)

# In some Unix systems (e.g. Arch Linux) need to use RPATH (runtime path) to find SWI
# library from installed shared objects.
list(APPEND CMAKE_INSTALL_RPATH ${SWIPL_LIBRARY_DIRS})

# Crisp-specific include dirs
list(APPEND CRISP_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/include)
list(APPEND CRISP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

# TODO: move this to dirs where it's really necessary, i.e., with targets that need to
# compile with clang. This probably will solve the problem with distributions without
# clang++
set(CMAKE_CXX_COMPILER "${CLANG_EXECUTABLE}")
set(CMAKE_C_COMPILER "${CLANG_EXECUTABLE}")

# The following variables are used in subdirectories. They represent relative paths
# meant to be prefixed with either "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}" or
# ${CMAKE_BINARY_DIR}.
set(SWI_BOOT_FILES_DIR "SWIBootFiles")
set(CRISP_RULES_DIR "CrispRules")
set(PROLOG_RULES_DIR "PrologRules")

# Install, documentation files
install(FILES COPYING README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})

# Subdirectories
add_subdirectory(include)
add_subdirectory(prolog)
add_subdirectory(lib)
add_subdirectory(test)

# TODO: make clean
# TODO: make doc
