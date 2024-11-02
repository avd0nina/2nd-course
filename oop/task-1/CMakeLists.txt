cmake_minimum_required(VERSION 3.27)
project(23202_AVDONINA_OOP_CPP)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(23202_AVDONINA_OOP_CPP
        src/BitArray.cpp
        src/BitArray.h
        src/main.cpp
)

add_executable(BitArrayTest tests/BitArray_test.cpp src/BitArray.cpp)

target_link_libraries(BitArrayTest GTest::gtest_main)

include(GoogleTest)

gtest_discover_tests(BitArrayTest)
enable_testing()
