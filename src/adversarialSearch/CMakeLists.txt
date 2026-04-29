if(NOT EMSCRIPTEN)
	find_package(OpenMP REQUIRED)
endif()

add_library(adversarialSearch utilities.cpp)

target_include_directories(adversarialSearch PUBLIC include)
if(NOT EMSCRIPTEN)
	target_link_libraries(adversarialSearch PUBLIC OpenMP::OpenMP_CXX)
endif()
