add_library(tablut  state.cpp 
                    common.cpp 
                    actions.cpp 
                    result.cpp)

target_include_directories(tablut PUBLIC include)
