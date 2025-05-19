cmake_minimum_required(VERSION 3.10)
project(tablut_ai)

option(ENABLE_TESTING "Enable testing" OFF)
if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

add_subdirectory(src)