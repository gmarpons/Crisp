# output:
#   SWIPL_FOUND
#   SWIPL_EXECUTABLE
#   SWIPL_ROOT_DIR
#   SWIPL_INCLUDE_DIRS
#   SWIPL_LIBRARIES

function(set_swipl_root swipl_cmd)
  execute_process(
    COMMAND ${swipl_cmd} --dump-runtime-variables
    COMMAND sed -n "s:PLBASE=\"\\(.*\\)\";:\\1:p"
    OUTPUT_VARIABLE swipl_base_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if (swipl_base_dir MATCHES "^$")
    message(FATAL_ERROR "Can't get swipl base dir")
  endif()
  set(SWIPL_ROOT_DIR "${swipl_base_dir}" PARENT_SCOPE)
endfunction()

function(set_swipl_arch swipl_cmd)
  execute_process(
    COMMAND ${swipl_cmd} --dump-runtime-variables
    COMMAND sed -n "s:PLARCH=\"\\(.*\\)\";:\\1:p"
    OUTPUT_VARIABLE swipl_plarch
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(swipl_plarch MATCHES "^$")
    message(FATAL_ERROR "Can't get swipl library path")
  endif()
  set(SWIPL_ARCH "${swipl_plarch}" PARENT_SCOPE)
endfunction()

find_program(SWIPL_EXECUTABLE swipl
    PATH ${SWIPL_LOCATION})

if(SWIPL_EXECUTABLE)
  set_swipl_root(${SWIPL_EXECUTABLE})
  set_swipl_arch(${SWIPL_EXECUTABLE})
  message(STATUS "Swipl include dirs: ${SWIPL_INCLUDE_DIRS}")
  message(STATUS "Swipl libraries: ${SWIPL_LIBRARIES}")

  find_library(SWIPL_LIBRARIES libswipl.so
    PATH
      ${SWIPL_LOCATION}
      ${SWIPL_ROOT_DIR}/lib/${SWIPL_ARCH}
  )
  find_path(SWIPL_INCLUDE_DIRS SWI-Prolog.h
    PATH
      ${SWIPL_LOCATION}
      ${SWIPL_ROOT_DIR}/include
  )

endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SWIPL DEFAULT_MSG
  SWIPL_EXECUTABLE SWIPL_LIBRARIES SWIPL_INCLUDE_DIRS)
