find_package(GTest REQUIRED)

add_executable(simpleJson_test simpleJson_test.cpp)

foreach(target simpleJson_test)
  target_link_libraries(${target}
    PRIVATE
      GTest::GTest
      tablut
      serverConnection)
endforeach()

add_test(NAME simpleJson_test COMMAND simpleJson_test)