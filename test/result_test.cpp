#include <gtest/gtest.h>
#include <algorithm>

#include <result.h>

TEST(ResultTest, InitialState) {
    State initialState;
    Move move(cord(4, 3), cord(4, 2));
    State result;
    result = Result::applyAction(initialState, move);

    initialState.movePiece(move.getFrom(), move.getTo());
    initialState.setTurn(Turn::Black);
    std::cout << "Initial State:\n" << initialState.boardString() << std::endl;
    std::cout << "Result State:\n" << result.boardString() << std::endl;
    EXPECT_EQ(initialState.softHash(), result.softHash());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}