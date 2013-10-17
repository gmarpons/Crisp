# This file provides the following Cmake function:
#
#   find_llvm_and_clang(LLVM_REQUIRED_VERSION)
#
# that, given a required version of those software packages, tries to find them in the
# system following four different strategies:
#
# 1) Cmake variable LLVM_ROOT is defined and points to the root of a build tree for
# LLVM/Clang. Libraries and executables are taken from the the given build tree.
#
# 2) Cmake variable LLVM_ROOT is defined and points to the root of an install tree for
# LLVM/Clang. Libraries and executables are taken from the the given install tree.
#
# 3) Cmake variable LLVM_ROOT is not defined, but we are compiling from a directory
# LLVM_BUILD_TREE/projects/crisp, for some root directory LLVM_BUILD_TREE. This is the
# default place for compiling LLVM projects. Libraries and executables are taken from
# the build tree we are in.
#
# 4) Cmake variable LLVM_ROOT is not defined, and we are not compiling from inside a
# build tree for LLVM, but a system-wide and PATH-accessible version of the required
# tools and libraries is found, and we use it.
#
# If a different Clang version is needed, a second optional argument with the required
# version can be given to function `find_llvm_and_clang'. Otherwise, The same version is
# used for LLVM and Clang.
#
# The function sets the following variables:
#
#   LLVM_VERSION
#
#   LLVM-CONFIG_EXECUTABLE
#
#   LLVM_TOOLS_BINARY_DIR
#
#   LLVM_INCLUDE_DIRS
#
#   LLVM_DEFINITIONS
#
#   LLVM_BUILD_TYPE
#
#   CLANG_VERSION
#
#   CLANG_EXECUTABLE
#
#   CLANG_INCLUDE_DIRS
#
#   CLANG_DEFINITIONS

# TODO: Allow a list of compatible versions for both LLVM and Clang.

function(find_llvm_and_clang LLVM_REQUIRED_VERSION)
  # Unset some variables that we want to derive from other settings.
  unset(LLVM_DIR CACHE)         # TODO: what's this for?
  unset(LLVM-CONFIG_EXECUTABLE CACHE)
  unset(CLANG_EXECUTABLE CACHE)

  # If an optional argument is given, take it as the required version for Clang. Take
  # same version for LLVM and Clang, otherwise.
  if(${ARGC} GREATER 1)
    set(CLANG_REQUIRED_VERSION "${ARGV1}")
  else(${ARGC} GREATER 1)
    set(CLANG_REQUIRED_VERSION "${LLVM_REQUIRED_VERSION}")
  endif(${ARGC} GREATER 1)

  if(DEFINED LLVM_ROOT)
    # WARNING: If LLVM_ROOT is defined as a relative path, it is taken to be relative to
    # the *source* tree root.
    get_filename_component(LLVM_ROOT_ABSOLUTE "${LLVM_ROOT}" ABSOLUTE)
    # Check if using an installed version of LLVM
    set(EXAMPLE_FILE_IN_LLVM_BUILD_TREE "${LLVM_ROOT_ABSOLUTE}/cmake_install.cmake")
    if(EXISTS ${EXAMPLE_FILE_IN_LLVM_BUILD_TREE})

      # Case 1: LLVM_ROOT defined and use of a non-installed (build tree) version of
      # LLVM

      set_variables_from_llvm_build_tree(${LLVM_ROOT_ABSOLUTE})
    else(EXISTS ${EXAMPLE_FILE_IN_LLVM_BUILD_TREE})

      # Case 2: LLVM_ROOT defined and use of an installed version of LLVM

      list(APPEND CMAKE_PREFIX_PATH "${LLVM_ROOT_ABSOLUTE}")
      set_variables_from_installed_llvm()
    endif(EXISTS ${EXAMPLE_FILE_IN_LLVM_BUILD_TREE})
  else(DEFINED LLVM_ROOT)
    # Check if we're in llvm-build-tree/projects/crisp
    get_filename_component(PROJECT_NAME "${CMAKE_CURRENT_BINARY_DIR}" NAME)
    get_filename_component(LLVM_PROJECTS_ROOT "${CMAKE_CURRENT_BINARY_DIR}" PATH)
    get_filename_component(PROJECTS_DIR "${LLVM_PROJECTS_ROOT}" NAME)
    # CRISP_RELATIVE_PATH is relative to LLVM_ROOT
    set(CRISP_RELATIVE_PATH "${PROJECTS_DIR}/${PROJECT_NAME}")
    if("${CRISP_RELATIVE_PATH}" STREQUAL "projects/crisp")

      # Case 3: LLVM_ROOT not defined and use of a non-installed version of LLVM (we're
      # in LLVM build tree, standard crisp location)

      get_filename_component(LLVM_ROOT_ABSOLUTE "${LLVM_PROJECTS_ROOT}" PATH)
      set_variables_from_llvm_build_tree(${LLVM_ROOT_ABSOLUTE})
    else("${CRISP_RELATIVE_PATH}" STREQUAL "projects/crisp")

      # Case 4: LLVM_ROOT not defined and use of an installed version (system-wide and
      # PATH-accessible) of LLVM

      # As CMAKE_PREFIX_PATH is not set, the following function takes
      # llvm-config and clang executable files from the PATH
      # environment var.
      set_variables_from_installed_llvm()
    endif("${CRISP_RELATIVE_PATH}" STREQUAL "projects/crisp")
  endif(DEFINED LLVM_ROOT)

  # Check that needed executables have been found and that LLVM and
  # Clang versions are correct.

  if(NOT LLVM-CONFIG_EXECUTABLE)
    message(FATAL_ERROR "Executable llvm-config not found.")
  endif(NOT LLVM-CONFIG_EXECUTABLE)

  if(NOT CLANG_EXECUTABLE)
    message(FATAL_ERROR "Executable clang not found.")
  endif(NOT CLANG_EXECUTABLE)

  if(NOT ${LLVM_VERSION} VERSION_EQUAL ${LLVM_REQUIRED_VERSION})
    message(
      FATAL_ERROR
      "Could NOT find LLVM version ${LLVM_REQUIRED_VERSION} (found version ${LLVM_VERSION})"
      )
  endif(NOT ${LLVM_VERSION} VERSION_EQUAL ${LLVM_REQUIRED_VERSION})
  if(NOT ${LLVM_VERSION} STREQUAL ${LLVM_REQUIRED_VERSION})
    message(
      WARNING "LLVM version found (${LLVM_VERSION}) slightly different from needed (${LLVM_REQUIRED_VERSION})"
      )
  endif(NOT ${LLVM_VERSION} STREQUAL ${LLVM_REQUIRED_VERSION})

  if(NOT ${CLANG_VERSION} VERSION_EQUAL ${CLANG_REQUIRED_VERSION})
    message(
      FATAL_ERROR
      "Could NOT find Clang version ${CLANG_REQUIRED_VERSION} (found version ${CLANG_VERSION})"
      )
  endif(NOT ${CLANG_VERSION} VERSION_EQUAL ${CLANG_REQUIRED_VERSION})
  # CLANG_VERSION's possible suffix is stripped out in functions `set_variables_from*'.

  execute_process(
    COMMAND ${LLVM-CONFIG_EXECUTABLE} --build-mode
    RESULT_VARIABLE LLVM-CONFIG_RESULT
    OUTPUT_VARIABLE LLVM_BUILD_TYPE
    )
  string(STRIP "${LLVM_BUILD_TYPE}" LLVM_BUILD_TYPE)
  set(LLVM_BUILD_TYPE ${LLVM_BUILD_TYPE} CACHE INTERNAL "")
  message(STATUS "LLVM version: ${LLVM_VERSION}")
  message(STATUS "Clang version: ${CLANG_VERSION}")
endfunction(find_llvm_and_clang)

function(set_variables_from_llvm_build_tree LLVM_ROOT_ABSOLUTE)
  list(APPEND CMAKE_PREFIX_PATH ${LLVM_ROOT_ABSOLUTE})
  # The following command sets LLVM_TOOLS_BINARY_DIR, LLVM_*_VERSION_*, LLVM_DEFINITIONS
  # Package found through CMAKE_PREFIX_PATH
  find_package(LLVM REQUIRED)
  set(LLVM_TOOLS_BINARY_DIR ${LLVM_TOOLS_BINARY_DIR} CACHE INTERNAL "")
  set(LLVM_VERSION ${LLVM_VERSION} CACHE INTERNAL "")
  set(LLVM_DEFINITIONS ${LLVM_DEFINITIONS} CACHE INTERNAL "")
  # Set LLVM-CONFIG_EXECUTABLE
  # Executable found through CMAKE_PREFIX_PATH, set above
  find_program(LLVM-CONFIG_EXECUTABLE NAMES llvm-config)
  # Set LLVM_INCLUDE_DIRS
  execute_process(
    COMMAND ${LLVM-CONFIG_EXECUTABLE} --src-root
    RESULT_VARIABLE LLVM-CONFIG_RESULT
    OUTPUT_VARIABLE LLVM_SRC_ROOT
    )
  string(STRIP "${LLVM_SRC_ROOT}" LLVM_SRC_ROOT_STRIP)
  set(LLVM_INCLUDE_DIRS
    "${LLVM_ROOT_ABSOLUTE}/include"
    "${LLVM_SRC_ROOT_STRIP}/include"
    CACHE INTERNAL ""
    )
  # Set CLANG_VERSION, assume standard directories for Clang
  set(CLANG_SRC_ROOT "${LLVM_SRC_ROOT_STRIP}/tools/clang")
  set(CLANG_OBJ_ROOT "${LLVM_ROOT_ABSOLUTE}/tools/clang")
  set(EXAMPLE_FILE_IN_CLANG_BUILD_TREE "${CLANG_OBJ_ROOT}/cmake_install.cmake")
  if(NOT EXISTS ${EXAMPLE_FILE_IN_CLANG_BUILD_TREE})
    message(FATAL_ERROR "Clang build tree not found in ${CLANG_OBJ_ROOT}")
  endif()
  find_program(CLANG_EXECUTABLE
    NAMES clang
    HINTS ${LLVM_TOOLS_BINARY_DIR}
    )
  execute_process(
    COMMAND ${CLANG_EXECUTABLE} --version
    RESULT_VARIABLE CLANG_RESULT
    OUTPUT_VARIABLE CLANG_VERSION_DIRTY
    )
  string(
    REGEX REPLACE "clang version ([0-9]+\\.[0-9]+).*"
    "\\1"
    CLANG_VERSION
    ${CLANG_VERSION_DIRTY}
    )
  set(CLANG_VERSION ${CLANG_VERSION} CACHE INTERNAL "")
  # Set CLANG_INCLUDE_DIRS
  set(CLANG_INCLUDE_DIRS
    "${CLANG_OBJ_ROOT}/include"
    "${CLANG_SRC_ROOT}/include"
    CACHE INTERNAL ""
    )
endfunction(set_variables_from_llvm_build_tree)

function(set_variables_from_installed_llvm)
  # If CMAKE_PREFIX_PATH contains LLVM_ROOT_ABSOLUTE, the following
  # executable is taken from that directory. Otherwise, it is taken
  # from the PATH environment var.
  find_program(LLVM-CONFIG_EXECUTABLE NAMES llvm-config)
  execute_process(
    COMMAND ${LLVM-CONFIG_EXECUTABLE} --bindir
    RESULT_VARIABLE LLVM-CONFIG_RESULT
    OUTPUT_VARIABLE LLVM_TOOLS_BINARY_DIR
    )
  string(STRIP "${LLVM_TOOLS_BINARY_DIR}" LLVM_TOOLS_BINARY_DIR)
  set(LLVM_TOOLS_BINARY_DIR ${LLVM_TOOLS_BINARY_DIR} CACHE INTERNAL "")
  execute_process(
    COMMAND ${LLVM-CONFIG_EXECUTABLE} --version
    RESULT_VARIABLE LLVM-CONFIG_RESULT
    OUTPUT_VARIABLE LLVM_VERSION
    )
  string(STRIP "${LLVM_VERSION}" LLVM_VERSION)
  set(LLVM_VERSION ${LLVM_VERSION} CACHE INTERNAL "")
  # If CMAKE_PREFIX_PATH contains LLVM_ROOT_ABSOLUTE, the following
  # executable is taken from that directory. Otherwise, it is taken
  # from the PATH environment var.
  find_program(CLANG_EXECUTABLE
    NAMES clang
    HINTS ${LLVM_TOOLS_BINARY_DIR}
    )
  execute_process(
    COMMAND ${CLANG_EXECUTABLE} --version
    RESULT_VARIABLE CLANG_RESULT
    OUTPUT_VARIABLE CLANG_VERSION_DIRTY
    )
  string(
    REGEX REPLACE "clang version ([0-9]+\\.[0-9]+).*"
    "\\1"
    CLANG_VERSION
    ${CLANG_VERSION_DIRTY}
    )
  set(CLANG_VERSION ${CLANG_VERSION} CACHE INTERNAL "")
  # Set LLVM_INCLUDE_DIRS and CLANG_INCLUDE_DIRS
  execute_process(
    COMMAND ${LLVM-CONFIG_EXECUTABLE} --includedir
    RESULT_VARIABLE LLVM-CONFIG_RESULT
    OUTPUT_VARIABLE LLVM_INCLUDE_DIRS
    )
  string(STRIP "${LLVM_INCLUDE_DIRS}" LLVM_INCLUDE_DIRS)
  set(LLVM_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS} CACHE INTERNAL "")
  set(CLANG_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS} CACHE INTERNAL "")
  # Set LLVM_DEFINITIONS, copied from
  # LLVM_OBJ_ROOT/share/cmake/LLVMConfig.cmake
  set(LLVM_DEFINITIONS
    "-D__STDC_LIMIT_MACROS" "-D__STDC_CONSTANT_MACROS"
    CACHE INTERNAL ""
    )
endfunction(set_variables_from_installed_llvm)
