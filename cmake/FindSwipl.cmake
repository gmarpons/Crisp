# - Try to find the SWI Prolog system commands and libraries.
#
# This module accepts the following variables, that are available both
# as CMake variables and environment variables (if both versions of
# the variable are defined, the CMake version takes precedence). All
# of them are optional, and only provided to help CMake to find SWI
# Prolog in non-standard locations.
#
#   SWIPL_BINS          Search path used to find the SWI Prolog
#                       binaries. If defined, it takes precedence over
#                       variable $PATH. Standard locations for SWI
#                       /lib and /include directories are found
#                       through SWI Prolog commands.
#
#   SWIPL_INCLUDES      Search path used to find the SWI Prolog
#                       include files. If defined, it takes precedence
#                       over the standard location as computed by the
#                       swipl command.
#
#   SWIPL_LIBS          Search path used to find the SWI Prolog
#                       libraries. If defined, it takes precedence
#                       over the standard location as computed by the
#                       swipl command.
#
# This module defines the following variables:
#
#   SWIPL_FOUND         Whether SWI Prolog binary, library and include
#                       files where found in the system.
#
#   SWIPL_EXECUTABLE    Absolute path for SWI Prolog main executable.
#
#   SWIPL_INCLUDE_DIR   SWI Prolog include directory.
#
#   SWIPL_LIBRARIES     Link this to embed SWI Prolog in another program.

# Sets global variable SWIPL_ROOT_DIR with the base dir of the SWI
# Prolog installation.
function(set_swipl_root_dir SWIPL_CMD)
  execute_process(
    COMMAND ${SWIPL_CMD} --dump-runtime-variables
    COMMAND sed -n "s:PLBASE=\"\\(.*\\)\";:\\1:p"
    OUTPUT_VARIABLE SWIPL_BASE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  if (SWIPL_BASE_DIR MATCHES "^$")
    message(FATAL_ERROR "Can't get swipl base dir")
  endif()
  set(SWIPL_ROOT_DIR "${SWIPL_BASE_DIR}" PARENT_SCOPE)
endfunction(set_swipl_root_dir)

# Sets global variable SWIPL_ARCH with a string identifying the
# architecture of the found SWI Prolog installation.
function(set_swipl_arch SWIPL_CMD)
  execute_process(
    COMMAND ${SWIPL_CMD} --dump-runtime-variables
    COMMAND sed -n "s:PLARCH=\"\\(.*\\)\";:\\1:p"
    OUTPUT_VARIABLE SWIPL_PLARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  if(SWIPL_PLARCH MATCHES "^$")
    message(FATAL_ERROR "Can't get swipl library path")
  endif()
  set(SWIPL_ARCH "${SWIPL_PLARCH}" PARENT_SCOPE)
endfunction(set_swipl_arch)

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
  # set_swipl_version(${SWIPL_EXECUTABLE})

  find_library(SWIPL_LIBRARIES swipl
    HINTS
      ${SWIPL_LIBS}
      ENV SWIPL_LIBS
    PATHS
      ${SWIPL_ROOT_DIR}/lib/${SWIPL_ARCH}
      )

  # Look for SWI C-interface library.
  find_path(SWIPL_INCLUDE_DIR SWI-Prolog.h
    HINTS
      ${SWIPL_INCLUDES}
      ENV SWIPL_INCLUDES
      ${SWIPL_ROOT_DIR}/include
      )

  message(STATUS "Swipl include dir: ${SWIPL_INCLUDE_DIR}")
  message(STATUS "Swipl libraries: ${SWIPL_LIBRARIES}")
endif()

# Handle the QUIETLY and REQUIRED arguments and set SWIPL_FOUND to
# TRUE if all listed variables are TRUE
find_package_handle_standard_args(SWIPL
  REQUIRED_VARS SWIPL_EXECUTABLE SWIPL_LIBRARIES SWIPL_INCLUDE_DIR
)

# TODO: version
# TODO: sed -> cmake regexp
# TODO: add try-compile