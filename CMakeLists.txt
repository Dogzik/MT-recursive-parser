cmake_minimum_required(VERSION 3.1)
project(LAB2)

set(CMAKE_CXX_STANDARD 17)

include_directories(third_party/json)
include_directories(third_party/googletest)
include_directories(third_party/googletest/include)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)

set(SOURCES lexer.h lexer.cpp parser.cpp parser.h)
file(GLOB GTEST_SRC "third_party/googletest/src/*")

add_executable(parser main.cpp ${SOURCES})
add_executable(parser_tests tests/tests.cpp ${SOURCES})
add_library(gtest STATIC ${GTEST_SRC})

target_link_libraries(parser_tests gtest pthread)