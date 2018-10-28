cmake_minimum_required(VERSION 3.1)
project(LAB2)

set(CMAKE_CXX_STANDARD 17)

include_directories(third_party/json)

set(SOURCES lexer.h lexer.cpp parser.cpp parser.h)

add_executable(parser main.cpp ${SOURCES})

add_subdirectory(tests)