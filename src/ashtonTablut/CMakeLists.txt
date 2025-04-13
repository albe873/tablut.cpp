add_library(tablut  state.cpp 
                    common.cpp 
                    actions.cpp 
                    result.cpp
                    game.cpp
                    heuristics.cpp)

target_include_directories(tablut PUBLIC include)

target_link_libraries(tablut adversarialSearch)
