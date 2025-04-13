add_executable(randomClient randomClient.cpp)
target_link_libraries(randomClient tablut
                                   serverConnection
)


find_package(OpenMP REQUIRED)

add_executable(aiPlayer aiPlayer.cpp)
target_link_libraries(aiPlayer  tablut
                                serverConnection
                                adversarialSearch
                                OpenMP::OpenMP_CXX
)
