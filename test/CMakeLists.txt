find_package(GTest REQUIRED)

add_executable(state_test state_test.cpp)
add_executable(actions_test actions_test.cpp)
foreach(target state_test actions_test)
  target_link_libraries(${target}
    PRIVATE
      GTest::GTest
      tablut)
endforeach()

add_test(NAME state_test COMMAND state_test)
add_test(NAME actions_test COMMAND actions_test)