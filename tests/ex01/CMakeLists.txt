enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/ex00)

add_executable(
  ex00_test
  ${CMAKE_SOURCE_DIR}/ex00/BitcoinExchange.cpp
  ${CMAKE_SOURCE_DIR}/tests/ex00/ex00.test.cpp
)
target_link_libraries(
  ex00_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ex00_test)
