find_package(OpenMP REQUIRED)

add_library(adversarialSearch   utilities.cpp)

target_include_directories(adversarialSearch PUBLIC include)
target_link_libraries(adversarialSearch PUBLIC OpenMP::OpenMP_CXX)
