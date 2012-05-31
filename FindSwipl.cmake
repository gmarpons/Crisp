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
  string(REGEX MATCH "([0-9]+(\\.[0-9]+)*)" version ${swipl_version})
  set(SWIPL_VERSION_STRING "${version}" PARENT_SCOPE)
endfunction()

function(set_swipl_include_flags swipl_cmd)
  execute_process(
    COMMAND ${swipl_cmd} --dump-runtime-variables
    COMMAND sed -n "s:PLBASE=\"\\(.*\\)\";:\\1:p"
    OUTPUT_VARIABLE swipl_base_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(SWIPL_ROOT_DIR "${swipl_base_dir}" PARENT_SCOPE)
  set(SWIPL_INCLUDE_DIRS "${swipl_base_dir}/include" PARENT_SCOPE)
endfunction()

function(set_swipl_definitions_flags swipl_cmd)
  execute_process(
    COMMAND ${swipl_cmd} --dump-runtime-variables
    COMMAND sed -n "s:PLCFLAGS=\"\\(.*\\)\";:\\1:p"
    OUTPUT_VARIABLE swipl_cflags
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REGEX MATCHALL "(-D[^ ]*)" cxxflags ${swipl_cflags})
  set(SWIPL_DEFINITIONS ${cxxflags} PARENT_SCOPE)
endfunction()

function(set_swipl_libraries swipl_cmd)
  execute_process(
    COMMAND ${swipl_cmd} --dump-runtime-variables
    COMMAND sed -n "s:PLLIBS=\"\\(.*\\)\";:\\1:p"
    OUTPUT_VARIABLE swipl_libs
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REGEX MATCHALL "(-l[^ ]*)" libs ${swipl_libs})
  set(SWIPL_LIBRARIES ${libs} PARENT_SCOPE)
endfunction()

function(set_swipl_library_dirs swipl_cmd)
  execute_process(
    COMMAND ${swipl_cmd} --dump-runtime-variables
    COMMAND sed -n "s:PLLDFLAGS=\"\\(.*\\)\";:\\1:p"
    OUTPUT_VARIABLE swipl_ldflags
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REGEX MATCH "(-rpath=[^ ]*)" rpathflags ${swipl_ldflags})
  string(REGEX REPLACE "-rpath=([^ ]*)" "\\1" ldflags ${rpathflags})
  set(SWIPL_LIBRARY_DIRS ${ldflags} PARENT_SCOPE)
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
