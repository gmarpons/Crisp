# If LLVM's lit found, enable regression tests with lit
if(LIT_FOUND)
  # Prepare build directory for the use of lit
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
    ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
    @ONLY)

  # Add a `check' target. We use add_custom_target instead of add_test because
  # dependencies cannot be added to a target created with add_test, due to a bug in
  # Cmake: http://public.kitware.com/Bug/view.php?id=8774, and because llvm-lit an CTest
  # overlap in some of their functionality, and it's not necessary to use both.
  add_custom_target(check
    COMMAND ${PYTHON_EXECUTABLE} ${LIT_EXECUTABLE} -v --no-progress-bar .
    COMMENT "Running regression tests")
  add_dependencies(check crispclang)
  add_dependencies(check crispllvm)
  message(STATUS "Generated `check' target to run regression tests with lit")
else(LIT_FOUND)
  message(STATUS "LLVM's lit not found, no `check' target generated")
endif(LIT_FOUND)

