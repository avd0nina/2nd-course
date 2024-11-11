cmake_minimum_required(VERSION 3.20)
project(23202-AVDONINA-OOP-CPP)
set(CMAKE_CXX_STANDARD 17)
set(GOOGLETEST_VERSION 1.14.0)

add_subdirectory(googletest/googletest)

enable_testing()

add_executable(23202-AVDONINA-OOP-CPP main.cpp view/view.h view/view.cpp model/model.h model/model.cpp controller/controller.h controller/controller.cpp
)

target_link_libraries(23202-AVDONINA-OOP-CPP gtest_main)
