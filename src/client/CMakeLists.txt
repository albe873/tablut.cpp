add_executable(randomClient randomClient.cpp)
target_link_libraries(randomClient tablut
                                   serverConnection
)

add_executable(aiPlayer aiPlayer.cpp)
target_link_libraries(aiPlayer  tablut
                                serverConnection
                                adversarialSearch
)

add_executable(aiPlayer2 aiPlayer2.cpp)
target_link_libraries(aiPlayer2 tablut
                                serverConnection
                                adversarialSearch
)
target_compile_definitions(aiPlayer2 PRIVATE ENABLE_METRICS)
target_compile_definitions(aiPlayer  PRIVATE ENABLE_METRICS)
