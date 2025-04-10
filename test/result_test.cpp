#include <gtest/gtest.h>
#include <algorithm>

#include <result.h>

TEST(ResultTest, MoveAndCapture) {
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

TEST(ResultTest, CaptureWithCampBlack) {
    State initialState;
    // Set up a scenario where a black piece captures a white piece using a camp
    initialState.board[0][3] = Piece::Empty;
    initialState.board[1][3] = Piece::White;

    Move move(cord(0, 4), cord(3, 2));
    State result = Result::applyAction(initialState, move);

    // Verify that the white piece was captured
    EXPECT_EQ(result.getPiece(cord(3, 1)), Piece::Empty);
}

TEST(ResultTest, CaptureWithCampWhite) {
    State initialState;
    // Set up a scenario where a white piece captures a black piece using a camp
    initialState.board[0][3] = Piece::Empty;
    initialState.board[1][3] = Piece::Black;

    Move move(cord(3, 4), cord(3, 2));
    State result = Result::applyAction(initialState, move);

    // Verify that the black piece was captured
    EXPECT_EQ(result.getPiece(cord(3, 1)), Piece::Empty);
}

TEST(ResultTest, CaptureWithCampAndKing) {
    State initialState;
    // Set up a scenario where a white piece captures a black piece using a camp and the king
    initialState.board[0][3] = Piece::Empty;
    initialState.board[1][3] = Piece::Black;

    Move move(cord(4, 4), cord(3, 2));
    State result = Result::applyAction(initialState, move);

    // Verify that the black piece was captured
    EXPECT_EQ(result.getPiece(cord(3, 1)), Piece::Empty);
}

TEST(ResultTest, CaptureKingWithCamp) {
    State initialState;
    // Set up a scenario where a white piece captures the king using a camp
    initialState.board[0][3] = Piece::Empty;
    initialState.board[1][3] = Piece::King;

    Move move(cord(0, 4), cord(3, 2));
    State result = Result::applyAction(initialState, move);

    EXPECT_EQ(result.getTurn(), Turn::BlackWin);
}

TEST(ResultTest, CaptureKingInThrone) {
    State initialState;
    initialState.board[4][3] = Piece::Black;
    initialState.board[4][5] = Piece::Black;
    initialState.board[3][4] = Piece::Black;

    Move move(cord(0, 4), cord(4, 5));
    State result = Result::applyAction(initialState, move);

    EXPECT_EQ(result.getTurn(), Turn::BlackWin);
}

TEST(ResultTest, CaptureKingNearThrone) {
    State initialState;
    initialState.board[4][3] = Piece::King;
    initialState.board[5][3] = Piece::Black;
    initialState.board[3][3] = Piece::Black;

    Move move(cord(0, 4), cord(2, 4));
    State result = Result::applyAction(initialState, move);

    EXPECT_EQ(result.getTurn(), Turn::BlackWin);
}

TEST(ResultTest, CaptureNearThroneBlack) {
    State initialState;
    initialState.board[4][4] = Piece::Empty;
    initialState.board[5][4] = Piece::Black;

    Move move(cord(4, 3), cord(4, 6));
    State result = Result::applyAction(initialState, move);

    std::cout << "Initial State:\n" << initialState.boardString() << std::endl;
    std::cout << "Result State 2:\n" << result.boardString() << std::endl;

    ASSERT_TRUE(result.isEmpty(cord(4, 5)));
}

TEST(ResultTest, CaptureNearThroneWhite) {
    State initialState;
    initialState.board[4][4] = Piece::Empty;
    initialState.board[5][4] = Piece::White;

    Move move(cord(0, 4), cord(4, 6));
    State result = Result::applyAction(initialState, move);

    std::cout << "Initial State:\n" << initialState.boardString() << std::endl;
    std::cout << "Result State 2:\n" << result.boardString() << std::endl;

    ASSERT_TRUE(result.isEmpty(cord(4, 5)));
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}