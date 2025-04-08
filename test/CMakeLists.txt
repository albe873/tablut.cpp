find_package(GTest REQUIRED)

add_executable(tablut_tests state_test.cpp)
target_link_libraries(tablut_tests 
 PRIVATE
  GTest::GTest
  tablut)

add_test(NAME TablutTests COMMAND tablut_tests)