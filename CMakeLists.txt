cmake_minimum_required(VERSION 3.10)
project(tablut_ai)

# Enable Link-Time Optimization (LTO) globally for all targets
include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output)
if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO/LTO is not supported: ${output}")
endif()

option(ENABLE_TESTING "Enable testing" OFF)
if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

add_subdirectory(src)