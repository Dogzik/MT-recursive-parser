include_directories(../third_party/googletest)
include_directories(../third_party/googletest/include)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib)
file(GLOB GTEST_SRC "../third_party/googletest/src/*")

set(TMP)

foreach(file ${SOURCES})
    list(APPEND TMP ../${file})
endforeach()

add_executable(parser_tests tests.cpp ${TMP})


add_library(gtest STATIC ${GTEST_SRC})

target_link_libraries(parser_tests gtest pthread)

