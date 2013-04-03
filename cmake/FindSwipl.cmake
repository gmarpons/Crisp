# - Try to find the SWI Prolog system commands and libraries.
#
# This module accepts the following variables, that are available both
# as CMake variables and environment variables (if both versions of
# the variable are defined, the CMake version takes precedence). All
# of them are optional, and only provided to help CMake to find SWI
# Prolog in non-standard locations.
#
#   SWIPL_BINS            Search path used to find the SWI Prolog
#                         binaries. If defined, it takes precedence
#                         over variable $PATH. Standard locations for
#                         SWI /lib and /include directories are found
#                         through SWI Prolog commands.
#                        
#   SWIPL_INCLUDES        Search path used to find the SWI Prolog
#                         include files. If defined, it takes
#                         precedence over the standard location as
#                         computed by the swipl command.
#                        
#   SWIPL_LIBS            Search path used to find the SWI Prolog
#                         libraries. If defined, it takes precedence
#                         over the standard location as computed by
#                         the swipl command.
#
# This module defines the following variables:
#
#   SWIPL_FOUND           Whether SWI Prolog binary, library and
#                         include files where found in the system.
#                         
#   SWIPL_EXECUTABLE      Absolute path for SWI Prolog main
#                         executable.
#                         
#   SWIPL_INCLUDE_DIRS    SWI Prolog include directories. To be used
#                         with include_directories().
#                         
#   SWIPL_LIBRARIES       SWI Prolog libraries. To be used with
#                         target_link_libraries.
#
#   SWIPL_VERSION_STRING  SWI Prolog version in x.y.z format.

# Sets global variable SWIPL_ROOT_DIR with the base dir of the SWI
# Prolog installation.
function(set_swipl_root_dir SWIPL_CMD)
  set(SWIPL_HOME_GOAL "(current_prolog_flag(home, X), write(X))")
  execute_process(
    COMMAND ${SWIPL_CMD} -g ${SWIPL_HOME_GOAL} -t halt
    RESULT_VARIABLE SWIPL_HOME_RESULT
    OUTPUT_VARIABLE SWIPL_ROOT_DIR
    )
  if (NOT ${SWIPL_HOME_RESULT} EQUAL 0)
    message(FATAL_ERROR "Can't get swipl base dir")
  endif()
  set(SWIPL_ROOT_DIR ${SWIPL_ROOT_DIR} PARENT_SCOPE)
endfunction(set_swipl_root_dir)

# Sets global variable SWIPL_ARCH with a string identifying the
# architecture of the found SWI Prolog installation.
function(set_swipl_arch SWIPL_CMD)
  set(SWIPL_ARCH_GOAL "(current_prolog_flag(arch, X), write(X))")
  execute_process(
    COMMAND ${SWIPL_CMD} -g ${SWIPL_ARCH_GOAL} -t halt
    RESULT_VARIABLE SWIPL_ARCH_RESULT
    OUTPUT_VARIABLE SWIPL_ARCH
    )
  if(NOT ${SWIPL_ARCH_RESULT} EQUAL 0)
    message(FATAL_ERROR "Can't get swipl library path.")
  endif()
  set(SWIPL_ARCH "${SWIPL_ARCH}" PARENT_SCOPE)
endfunction(set_swipl_arch)

# Sets global variable SWIPL_VERSION_STRING
function(set_swipl_version SWIPL_CMD)
  set(SWIPL_VERSION_GOAL
    "(current_prolog_flag(version_data, swi(X,Y,Z,_)), format('~w.~w.~w', [X,Y,Z]))")
  execute_process(
    COMMAND ${SWIPL_CMD} -g ${SWIPL_VERSION_GOAL} -t halt
    RESULT_VARIABLE SWIPL_VERSION_RESULT
    OUTPUT_VARIABLE SWIPL_VERSION_STRING
    )
  if(NOT ${SWIPL_VERSION_RESULT} EQUAL 0)
    message(FATAL_ERROR "Can't get SWI Prolog version.")
  endif()
  set(SWIPL_VERSION_STRING "${SWIPL_VERSION_STRING}" PARENT_SCOPE)
endfunction(set_swipl_version)

# Main program for this module begins here
find_program(SWIPL_EXECUTABLE
  NAMES swipl
  HINTS
    ${SWIPL_BINS}
    ENV SWIPL_BINS
    )

if(SWIPL_EXECUTABLE)
  set_swipl_root_dir(${SWIPL_EXECUTABLE})
  set_swipl_arch(${SWIPL_EXECUTABLE})
  set_swipl_version(${SWIPL_EXECUTABLE})

  find_library(SWIPL_LIBRARIES swipl
    HINTS
      ${SWIPL_LIBS}
      ENV SWIPL_LIBS
    PATHS
      ${SWIPL_ROOT_DIR}/lib/${SWIPL_ARCH}
      )

  # Look for SWI C-interface library.
  find_path(SWIPL_INCLUDE_DIRS SWI-Prolog.h
    HINTS
      ${SWIPL_INCLUDES}
      ENV SWIPL_INCLUDES
      ${SWIPL_ROOT_DIR}/include
      )

  message(STATUS "SWI Prolog include dirs: ${SWIPL_INCLUDE_DIRS}")
  message(STATUS "SWI Prolog libraries: ${SWIPL_LIBRARIES}")

  # Try to compile SWI Prolog header files and a simple call to a SWI
  # C interface function
  include(CheckCXXSourceCompiles)
  set(SWIPL_TEST_SOURCE "
    #include <SWI-Prolog.h>
    int main(int argc, char **argv) { PL_initialise(argc, argv); return 0; }
  ")
  list(APPEND CMAKE_REQUIRED_INCLUDES ${SWIPL_INCLUDE_DIRS})
  list(APPEND CMAKE_REQUIRED_LIBRARIES ${SWIPL_LIBRARIES})
  check_cxx_source_compiles("${SWIPL_TEST_SOURCE}" SWIPL_TEST_COMPILES)
  if(NOT ${SWIPL_TEST_COMPILES})
    message(SEND_ERROR "Can't compile a test snippet calling SWI Prolog C interface")
  endif(NOT ${SWIPL_TEST_COMPILES})
endif()

# Handle the QUIETLY and REQUIRED arguments, handle version
# requirements, and set SWIPL_FOUND to TRUE if all listed variables
# are TRUE.
find_package_handle_standard_args(Swipl
  REQUIRED_VARS SWIPL_EXECUTABLE SWIPL_LIBRARIES SWIPL_INCLUDE_DIRS SWIPL_TEST_COMPILES
  VERSION_VAR SWIPL_VERSION_STRING
  )
