# Distributed under the MIT License (See accompanying file /LICENSE)
# Author(s): Bruno Schmitt
# CMake build : tweedledum library tests
add_compile_options(
  -fno-rtti
  -W
  -Wall
  -pedantic)

include_directories(catch2)

#  All Tests
#  =============================================================================
add_custom_target(tests COMMENT "Build all tests.")

set(tweedledum_tests_files
  "${CMAKE_SOURCE_DIR}/tests/networks/gates/qc_gate.cpp")

add_executable(run_tests EXCLUDE_FROM_ALL "${tweedledum_tests_files}")
target_link_libraries(run_tests tweedledum)
add_dependencies(tests run_tests)