add_library(serverConnection simpleJson.cpp
                             serverComunicator.cpp
)

target_include_directories(serverConnection PUBLIC include)

target_link_libraries(serverConnection tablut)