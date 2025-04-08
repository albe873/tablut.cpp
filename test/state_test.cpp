#include <gtest/gtest.h>
#include <state.h>

TEST(StateTest, placebo) {
    std::cout << "placebo test" << std::endl;
    std::string expected = 
     R"(0 0 0 B B B 0 0 0 
0 0 0 0 W 0 0 0 0 
B 0 0 0 W 0 0 0 B 
B B W W K W W B B 
B 0 0 0 W 0 0 0 B 
0 0 0 0 W 0 0 0 0 
0 0 0 0 B 0 0 0 0 
0 0 0 B B B 0 0 0 
)";

    std::string expected2 = 
     R"(0 0 0 B B B 0 0 0 
0 0 0 0 W 0 0 0 0 
B 0 0 0 W 0 0 0 B 
B B W W K W W B B 
B 0 0 0 W 0 0 0 B 
0 0 0 0 W 0 0 0 0 
0 0 0 0 B 0 0 0 0 
0 0 0 B B B 0 0 0 
)";
    ASSERT_TRUE(expected.compare(expected2) == 0); 
}

TEST(StateTest, InitialState) {
    std::cout << "Initial state test" << std::endl;
    State state = State();
    std::string expected = 
     R"(0 0 0 B B B 0 0 0 
0 0 0 0 B 0 0 0 0 
0 0 0 0 W 0 0 0 0 
B 0 0 0 W 0 0 0 B 
B B W W K W W B B 
B 0 0 0 W 0 0 0 B 
0 0 0 0 W 0 0 0 0 
0 0 0 0 B 0 0 0 0 
0 0 0 B B B 0 0 0 
)";

    std::cout << "state board toString\n" << std::endl;
    std::cout << state.boardString() << std::endl;
    std::cout << "expected board toString\n" << std::endl;
    std::cout << expected << std::endl;
    ASSERT_TRUE(expected.compare(state.boardString()) == 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}