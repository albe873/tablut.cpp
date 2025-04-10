#include <gtest/gtest.h>
#include <algorithm>

#include <result.h>

TEST(ResultTest, InitialState) {
    State initialState;
    Move move(cord(4, 3), cord(7, 3));
    State result;
    result = Result::applyAction(initialState, move);
    initialState.movePiece(move.getFrom(), move.getTo());

    std::cout << "Result State 1:\n" << result.boardString() << std::endl;

    ASSERT_TRUE(result.getTurn() == Turn::Black);

    move = Move(cord(0, 3), cord(6, 3));
    result = Result::applyAction(result, move);
    initialState.movePiece(move.getFrom(), move.getTo());
    initialState.removePiece(cord(7, 3));

    ASSERT_TRUE(result.getTurn() == Turn::White);

    std::cout << "Initial State:\n" << initialState.boardString() << std::endl;
    std::cout << "Result State 2:\n" << result.boardString() << std::endl;
    EXPECT_EQ(initialState.softHash(), result.softHash());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}