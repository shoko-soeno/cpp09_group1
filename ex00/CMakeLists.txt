project(ex00)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
option(TEST "ex00 test" OFF)

add_executable(
  ${PROJECT_NAME}
  main.cpp
  BitcoinExchange.cpp)

if (TEST)
  set(PROGRAM_NAME ex00_test.out)
  add_executable(
    ${PROGRAM_NAME}
    ex00.test.cpp
    BitcoinExchange.cpp
    )
  set_target_properties(
    ${PROGRAM_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ./tests)
  target_link_directories(
    ${PROGRAM_NAME}
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../packages/lib")
  target_link_libraries(
    ${PROGRAM_NAME}
    PRIVATE gtest gtest_main)
  target_include_directories(
    ${PROGRAM_NAME}
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../packages/include")

  add_test(NAME ${PROGRAM_NAME} COMMAND ${PROGRAM_NAME})
endif()
