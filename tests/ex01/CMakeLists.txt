enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/ex01)

add_executable(
  ex01_test
  ${CMAKE_SOURCE_DIR}/ex01/RPN.cpp
  ${CMAKE_SOURCE_DIR}/tests/ex01/ex01.test.cpp
)
target_link_libraries(
  ex01_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ex01_test)
