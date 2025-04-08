#include <gtest/gtest.h>

#include <state.h>

//#include "../src/state.cpp"

TEST(StateTest, InitialState) {
    State state = State();
    std::string expected = "";
    ASSERT_TRUE(expected.compare(state.boardString()) == 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}