enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/ex02)

add_executable(
  ex02_test
  ${CMAKE_SOURCE_DIR}/ex02/main.cpp
  ${CMAKE_SOURCE_DIR}/tests/ex02/ex02.test.cpp
)
target_link_libraries(
  ex02_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ex02_test)
