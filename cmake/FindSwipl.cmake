# output:
#   SWIPL_FOUND
#   SWIPL_EXECUTABLE
#   SWIPL_VERSION_STRING
#   SWIPL_ROOT_DIR
#   SWIPL_INCLUDE_DIRS
#   SWIPL_DEFINITIONS
#   SWIPL_LIBRARIES
#   SWIPL_LIBRARY_DIRS

function(set_swipl_version swipl_cmd)
  execute_process(
    COMMAND ${SWIPL_EXECUTABLE} --version
    OUTPUT_VARIABLE swipl_version
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT (swipl_version MATCHES "^$"))
    string(REGEX MATCH "([0-9]+(\\.[0-9]+)*)" version ${swipl_version})
    set(SWIPL_VERSION_STRING "${version}" PARENT_SCOPE)
  endif()
endfunction()

function(set_swipl_include_flags swipl_cmd)
  execute_process(
    COMMAND ${swipl_cmd} --dump-runtime-variables
    COMMAND sed -n "s:PLBASE=\"\\(.*\\)\";:\\1:p"
    OUTPUT_VARIABLE swipl_base_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if (swipl_base_dir MATCHES "^$")
    message(FATAL_ERROR "Can't get swipl base dir")
  else()
    set(SWIPL_ROOT_DIR "${swipl_base_dir}" PARENT_SCOPE)
    set(SWIPL_INCLUDE_DIRS "${swipl_base_dir}/include" PARENT_SCOPE)
  endif()
endfunction()

function(set_swipl_definitions_flags swipl_cmd)
  execute_process(
    COMMAND ${swipl_cmd} --dump-runtime-variables
    COMMAND sed -n "s:PLCFLAGS=\"\\(.*\\)\";:\\1:p"
    OUTPUT_VARIABLE swipl_cflags
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT (swipl_cflags MATCHES "^$"))
    string(REGEX MATCHALL "(-D[^ ]*)" cxxflags ${swipl_cflags})
    set(SWIPL_DEFINITIONS ${cxxflags} PARENT_SCOPE)
  endif()
endfunction()

function(set_swipl_libraries swipl_cmd)
  execute_process(
    COMMAND ${swipl_cmd} --dump-runtime-variables
    COMMAND sed -n "s:PLLIB=\"\\(.*\\)\";:\\1:p"
    OUTPUT_VARIABLE swipl_libs
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(swipl_libs MATCHES "^$")
    message(FATAL_ERROR "Can't get swipl library")
  else()
    string(REGEX MATCHALL "(-l[^ ]*)" libs ${swipl_libs})
    set(SWIPL_LIBRARIES ${libs} PARENT_SCOPE)
  endif()
endfunction()

function(set_swipl_library_dirs swipl_cmd)
  execute_process(
    COMMAND ${swipl_cmd} --dump-runtime-variables
    COMMAND sed -n "s:PLARCH=\"\\(.*\\)\";:\\1:p"
    OUTPUT_VARIABLE swipl_plarch
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(swipl_plarch MATCHES "^$")
    message(FATAL_ERROR "Can't get swipl library path")
  else()
    set(SWIPL_LIBRARY_DIRS "${SWIPL_ROOT_DIR}/lib/${swipl_plarch}" PARENT_SCOPE)
  endif()
endfunction()


find_program(SWIPL_EXECUTABLE swipl
    PATH ENV SWIPL_PATH)

if (SWIPL_EXECUTABLE)
  set_swipl_version(${SWIPL_EXECUTABLE})
  set_swipl_include_flags(${SWIPL_EXECUTABLE})
  set_swipl_definitions_flags(${SWIPL_EXECUTABLE})
  set_swipl_libraries(${SWIPL_EXECUTABLE})
  set_swipl_library_dirs(${SWIPL_EXECUTABLE})

  message(STATUS "Swipl version: ${SWIPL_VERSION_STRING}")
  message(STATUS "Swipl root dir: ${SWIPL_ROOT_DIR}")
  message(STATUS "Swipl include dirs: ${SWIPL_INCLUDE_DIRS}")
  message(STATUS "Swipl defines: ${SWIPL_DEFINITIONS}")
  message(STATUS "Swipl libraries: ${SWIPL_LIBRARIES}")
  message(STATUS "Swipl library dirs: ${SWIPL_LIBRARY_DIRS}")

  set(SWIPL_FOUND 1)
else()
  message(FATAL_ERROR "Swipl is not in PATH. Try to set SWIPL_PATH environment variable.")
endif()
