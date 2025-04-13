find_package(GTest REQUIRED)

add_executable(utilities_test utilities_test.cpp)

foreach(target utilities_test)
  target_link_libraries(${target}
    PRIVATE
      GTest::GTest
      tablut
      adversarialSearch)
endforeach()

add_test(NAME utilities_test COMMAND utilities_test)
