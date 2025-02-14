cmake_minimum_required(VERSION 3.29)
project(23202_AVDONINA_OOP_CPP)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(23202_AVDONINA_OOP_CPP
        src/FileReader.h
        src/FileReader.cpp
        src/Parser.h
        src/Parser.cpp
        src/Statistic.h
        src/Statistic.cpp
        src/CSVWriter.h
        src/CSVWriter.cpp
        src/main.cpp
)

add_executable(FileReaderTest src/FileReaderTest.cpp src/FileReader.cpp)
add_executable(ParserTest src/ParserTest.cpp src/Parser.cpp)
add_executable(StatisticTest src/StatisticTest.cpp src/Statistic.cpp)
add_executable(CSVWriterTest src/CSVWriterTest.cpp src/CSVWriter.cpp)

target_link_libraries(FileReaderTest GTest::gtest_main)
target_link_libraries(ParserTest GTest::gtest_main)
target_link_libraries(StatisticTest GTest::gtest_main)
target_link_libraries(CSVWriterTest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(FileReaderTest)
gtest_discover_tests(ParserTest)
gtest_discover_tests(StatisticTest)
gtest_discover_tests(CSVWriterTest)
enable_testing()
