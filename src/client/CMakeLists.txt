add_executable(randomClient randomClient.cpp)

target_link_libraries(randomClient tablut
                                   serverConnection
)

