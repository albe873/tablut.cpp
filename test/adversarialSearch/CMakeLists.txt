find_package(GTest REQUIRED)

add_executable(utilities_test utilities_test.cpp)
add_executable(timer_test timer_test.cpp)
add_executable(new_test new_test.cpp)
add_executable(t_table_test t_table_test.cpp)

foreach(target utilities_test timer_test new_test t_table_test)
  target_link_libraries(${target}
    PRIVATE
      GTest::GTest
      tablut
      adversarialSearch)
endforeach()

add_test(NAME utilities_test COMMAND utilities_test)
add_test(NAME t_table_test COMMAND t_table_test)
