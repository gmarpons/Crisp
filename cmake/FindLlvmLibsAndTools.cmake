# - Try to find LLVM libraries and main commands (opt, clang, llvm-config, lit).
#
# Important notice: this module does not make any attempt to find LLVM or Clang static or
# dynamic libraries. Only header files and executables are looked for.
#
# An LLVM version can be specified when calling find_package().
#
# This module tries to find LLVM libraries and tools following four different strategies,
# depending on the directory we are trying to build from, and if variable LLVM_ROOT is
# defined or not and its contents:
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
# The module sets the following variables:
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
#
#   LIT_EXECUTABLE
#
#   LIT_FOUND

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
  find_program(LLVM-CONFIG_EXECUTABLE
    NAMES llvm-config-${LlvmLibsAndTools_FIND_VERSION} llvm-config)

  # Set LLVM_INCLUDE_DIRS
  if(LLVM-CONFIG_EXECUTABLE)
    execute_process(
      COMMAND ${LLVM-CONFIG_EXECUTABLE} --src-root
      RESULT_VARIABLE LLVM-CONFIG_RESULT
      OUTPUT_VARIABLE LLVM_SRC_ROOT
      )
    if(${LLVM-CONFIG_RESULT} EQUAL 0)
      string(STRIP "${LLVM_SRC_ROOT}" LLVM_SRC_ROOT_STRIP)
      set(LLVM_INCLUDE_DIRS
        "${LLVM_ROOT_ABSOLUTE}/include"
        "${LLVM_SRC_ROOT_STRIP}/include"
        CACHE INTERNAL ""
        )

      # Set CLANG_EXECUTABLE and CLANG_VERSION, assume standard directories for Clang
      set(CLANG_SRC_ROOT "${LLVM_SRC_ROOT_STRIP}/tools/clang")
      set(CLANG_OBJ_ROOT "${LLVM_ROOT_ABSOLUTE}/tools/clang")
      set(EXAMPLE_FILE_IN_CLANG_BUILD_TREE "${CLANG_OBJ_ROOT}/cmake_install.cmake")
      if(EXISTS ${EXAMPLE_FILE_IN_CLANG_BUILD_TREE})
        find_program(CLANG_EXECUTABLE
          NAMES clang-${CLANG_FIND_VERSION} clang
          HINTS ${LLVM_TOOLS_BINARY_DIR}
          )
        if(CLANG_EXECUTABLE)
          execute_process(
            COMMAND ${CLANG_EXECUTABLE} --version
            RESULT_VARIABLE CLANG_RESULT
            OUTPUT_VARIABLE CLANG_VERSION_DIRTY
            )
          if(${CLANG_RESULT} EQUAL 0)
            # CLANG_VERSION's possible suffix is stripped out
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

            # Set (possibly optional) component as found
            set(LlvmLibsAndTools_clang_FOUND True PARENT_SCOPE)
          endif(${CLANG_RESULT} EQUAL 0)
        endif(CLANG_EXECUTABLE)
      endif(EXISTS ${EXAMPLE_FILE_IN_CLANG_BUILD_TREE})
    endif(${LLVM-CONFIG_RESULT} EQUAL 0)
  endif(LLVM-CONFIG_EXECUTABLE)
endfunction(set_variables_from_llvm_build_tree)

function(set_variables_from_installed_llvm)
  # If CMAKE_PREFIX_PATH contains LLVM_ROOT_ABSOLUTE, the following executable is taken
  # from that directory. Otherwise, it is taken from the PATH environment var.
  find_program(LLVM-CONFIG_EXECUTABLE
    NAMES llvm-config-${LlvmLibsAndTools_FIND_VERSION} llvm-config)

  if(LLVM-CONFIG_EXECUTABLE)

    # Set LLVM_TOOLS_BINARY_DIR and LLVM_VERSION
    execute_process(
      COMMAND ${LLVM-CONFIG_EXECUTABLE} --bindir
      RESULT_VARIABLE LLVM-CONFIG_RESULT
      OUTPUT_VARIABLE LLVM_TOOLS_BINARY_DIR
      )
    if(${LLVM-CONFIG_RESULT} EQUAL 0)
      string(STRIP "${LLVM_TOOLS_BINARY_DIR}" LLVM_TOOLS_BINARY_DIR)
      set(LLVM_TOOLS_BINARY_DIR ${LLVM_TOOLS_BINARY_DIR} CACHE INTERNAL "")
      execute_process(
        COMMAND ${LLVM-CONFIG_EXECUTABLE} --version
        RESULT_VARIABLE LLVM-CONFIG_RESULT
        OUTPUT_VARIABLE LLVM_VERSION
        )
      if(${LLVM-CONFIG_RESULT} EQUAL 0)
        string(STRIP "${LLVM_VERSION}" LLVM_VERSION)
        set(LLVM_VERSION ${LLVM_VERSION} CACHE INTERNAL "")

        # Set vars CLANG_EXECUTABLE and CLANG_VERSION, assume standard directories for
        # Clang. If CMAKE_PREFIX_PATH contains LLVM_ROOT_ABSOLUTE, the following
        # executable is taken from that directory. Otherwise, it is taken from the PATH
        # environment var.
        find_program(CLANG_EXECUTABLE
          NAMES clang-${CLANG_FIND_VERSION} clang
          HINTS ${LLVM_TOOLS_BINARY_DIR}
          )
        execute_process(
          COMMAND ${CLANG_EXECUTABLE} --version
          RESULT_VARIABLE CLANG_RESULT
          OUTPUT_VARIABLE CLANG_VERSION_DIRTY
          )
        if(${CLANG_RESULT} EQUAL 0)
          # CLANG_VERSION's possible suffix is stripped out
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
          if(${LLVM-CONFIG_RESULT} EQUAL 0)
            string(STRIP "${LLVM_INCLUDE_DIRS}" LLVM_INCLUDE_DIRS)
            set(LLVM_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS} CACHE INTERNAL "")
            set(CLANG_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS} CACHE INTERNAL "")

            # Set (possibly optional) component as found
            set(LlvmLibsAndTools_clang_FOUND True PARENT_SCOPE)

            # Set LLVM_DEFINITIONS, copied from LLVM_OBJ_ROOT/share/cmake/LLVMConfig.cmake
            set(LLVM_DEFINITIONS
              "-D__STDC_LIMIT_MACROS" "-D__STDC_CONSTANT_MACROS"
              CACHE INTERNAL ""
              )
          endif(${LLVM-CONFIG_RESULT} EQUAL 0)
        endif(${CLANG_RESULT} EQUAL 0)
      endif(${LLVM-CONFIG_RESULT} EQUAL 0)
    endif(${LLVM-CONFIG_RESULT} EQUAL 0)
  endif(LLVM-CONFIG_EXECUTABLE)
endfunction(set_variables_from_installed_llvm)

# Main program for this module begins here

# Unset some variables that we want to derive from other settings.
unset(LLVM_DIR CACHE)         # TODO: what's this for?
unset(LLVM-CONFIG_EXECUTABLE CACHE)
unset(CLANG_EXECUTABLE CACHE)

set(CLANG_FIND_VERSION "${LlvmLibsAndTools_FIND_VERSION}")

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

    # As CMAKE_PREFIX_PATH is not set, the following function takes llvm-config and clang
    # executable files from the PATH environment var.
    set_variables_from_installed_llvm()
  endif("${CRISP_RELATIVE_PATH}" STREQUAL "projects/crisp")
endif(DEFINED LLVM_ROOT)

# Get LLVM's build mode (e.g. "Debug", "Release")
execute_process(
  COMMAND ${LLVM-CONFIG_EXECUTABLE} --build-mode
  RESULT_VARIABLE LLVM-CONFIG_RESULT
  OUTPUT_VARIABLE LLVM_BUILD_TYPE
  )
if(${LLVM-CONFIG_RESULT} EQUAL 0)
  string(STRIP "${LLVM_BUILD_TYPE}" LLVM_BUILD_TYPE)
  set(LLVM_BUILD_TYPE ${LLVM_BUILD_TYPE} CACHE INTERNAL "")
endif(${LLVM-CONFIG_RESULT} EQUAL 0)

find_program(LIT_EXECUTABLE
  NAMES lit-${LlvmLibsAndTools_FIND_VERSION} lit llvm-lit
  HINTS ${LLVM_TOOLS_BINARY_DIR})
if(LIT_EXECUTABLE)
  # set(LIT_FOUND True CACHE INTERNAL "")
  # message(STATUS "LLVM's lit found : ${LIT_EXECUTABLE}")
  set(LlvmLibsAndTools_lit_FOUND True)
endif(LIT_EXECUTABLE)

message(STATUS "LLVM version: ${LLVM_VERSION}")
message(STATUS "LLVM root directory: ${LLVM_ROOT_ABSOLUTE}")
message(STATUS "LLVM build type: ${LLVM_BUILD_TYPE}")
if(LlvmLibsAndTools_clang_FOUND)
  message(STATUS "Clang executable: ${CLANG_EXECUTABLE}")
endif(LlvmLibsAndTools_clang_FOUND)
if(LlvmLibsAndTools_lit_FOUND)
  message(STATUS "LLVM's lit executable: ${LIT_EXECUTABLE}")
endif(LlvmLibsAndTools_lit_FOUND)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LlvmLibsAndTools
  FOUND_VAR LlvmLibsAndTools_FOUND # It's recommended to use OriginalCase_Name
  REQUIRED_VARS
    LLVM-CONFIG_EXECUTABLE LLVM_INCLUDE_DIRS LLVM_VERSION LLVM_DEFINITIONS LLVM_BUILD_TYPE
  VERSION_VAR LLVM_VERSION
  HANDLE_COMPONENTS
)
