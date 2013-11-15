# Usage notes:
#
# - Optional argument `-DLLVM_ROOT=/path/to/llvm/build/or/install/tree' can be passed to
#   Cmake to help it finding LLVM/Clang tools and libraries. See file
#   `cmake/FindLlvmLibsAndTools.cmake' for details about variable `LLVM_ROOT'.
# - Option `-DCMAKE_BUILD_TYPE=[Release|Debug]' can be used to set the build mode
#   (`Release' by default).
# - If Python interpreter version found by default can not be used to run regression
#   tests (version 2.7.* is needed), a warning is emitted. A different version can be
#   passed to Cmake with `-DPYTHON_EXECUTABLE=/path/to/python/interpreter'.
# - Clang compiler is needed to compile the C/C++ parts. By default, the same compiler
#   found by module `cmake/FindLlvmLibsAndTools.cmake' is used, but different binaries can
#   be defined when needed: `-DCMAKE_C_COMPILER=/path/to/c/compiler' and
#   `-DCMAKE_CXX_COMPILER=/path/to/cxx/compiler'
# - `DCMAKE_INSTALL_PREFIX=/install/path/' can be used as in any other project.

# Project name. We set 'NONE' as supported languages, as in most cases we're not going to
# use system's default C/C++ compiler. Support for C/C++ is enabled below in this file,
# after setting up a suitable C/C++ compiler.
project(crisp NONE)

# Required CMake version and policies
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

# Declare Crisp version number
set(Crisp_VERSION_MAJOR 0)
set(Crisp_VERSION_MINOR 2)
set(Crisp_VERSION_PATCH 0)

# Search for modules in source tree, before checking default CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Requisites and C/C++ compiler

# TODO: Update to LLVM 3.3.1 (probably dropping support for 3.2)
find_package(LlvmLibsAndTools 3.2     REQUIRED COMPONENTS clang OPTIONAL_COMPONENTS lit)

# Only Clang is going to work to compile Crisp. The user can give the compiler of her
# preference with -DCMAKE_[C|CXX]_COMPILER. We use the compiler found by package
# LlvmLibsAndTools otherwise, even if this is not a recommended way of defining the
# compiler (see http://www.cmake.org/Wiki/CMake_FAQ#How_do_I_use_a_different_compiler.3F)
if(NOT DEFINED CMAKE_C_COMPILER)
  set(CMAKE_C_COMPILER "${CLANG_EXECUTABLE}")
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER "${CLANG_EXECUTABLE}")
endif()

# C/C++ compilers are checked now
enable_language(C)
enable_language(CXX)

find_package(Boost            1.46.1  REQUIRED)
find_package(Swipl            6.2     REQUIRED)

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
