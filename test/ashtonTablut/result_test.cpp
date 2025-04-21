// result_test.cpp

#include <gtest/gtest.h>
#include <algorithm>

#include <tablut/result.h>

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

TEST(ResultTest, KingCaptureScenario) {
    // State provided by user where white moved f5->f6, resulting in King at (7,6)
    // and black should win in 1 move.
    State initialState;
    // Clear board and set pieces manually based on the provided state
    for (int r = 0; r < State::size; ++r) {
        for (int c = 0; c < State::size; ++c) {
            initialState.board[r][c] = Piece::Empty;
        }
    }

    // Row 0: - - - B B B - - -
    initialState.board[0][3] = Piece::Black; initialState.board[0][4] = Piece::Black; initialState.board[0][5] = Piece::Black;
    // Row 1: - - B - + - - - -
    initialState.board[1][2] = Piece::Black;
    // Row 2: - B - W - - - - -
    initialState.board[2][1] = Piece::Black; initialState.board[2][3] = Piece::White; 
    // Row 3: + - - - - W - - B
    initialState.board[3][5] = Piece::White; initialState.board[3][8] = Piece::Black;
    // Row 4: + + - - + - W B B
    initialState.board[4][6] = Piece::White; initialState.board[4][8] = Piece::Black;initialState.board[4][7] = Piece::Black;
    // Row 5: + - - - B W - - +
    initialState.board[5][4] = Piece::Black; initialState.board[5][5] = Piece::White;
    // Row 6: - - - B - - - K B
    initialState.board[6][3] = Piece::Black; initialState.board[6][7] = Piece::King; initialState.board[6][8] = Piece::Black;
    // Row 7: - - B - + - W B -
    initialState.board[7][2] = Piece::Black; initialState.board[7][6] = Piece::White; initialState.board[7][7] = Piece::Black;
    // Row 8: - - - + B + - - -
    initialState.board[8][4] = Piece::Black;

    // Manually update piece counts and king position
    //initialState.whiteP = 5;
    //initialState.blackP = 12; // Counted from above
    //initialState.kingPos = cord(7, 6);
    initialState.setTurn(Turn::Black); // It's Black's turn after White's move
    initialState.recalculateZobrist(); // Recalculate hash after manual changes
    initialState.clearHistory();       // Clear history for the test state

    // Black winning move: Move piece from (7, 4) to (7, 5) to surround king at (7, 6)
    // Note: Coordinates are (column, row)
    Move winningMove(cord(7, 4), cord(7, 5));

    // Apply the action
    State result = Result::applyAction(initialState, winningMove);

    // Assert that Black wins
    EXPECT_EQ(result.getTurn(), Turn::BlackWin);

    // Optional: Print boards for debugging
    std::cout << "Initial State (KingCaptureScenario):\n" << initialState.boardString() << std::endl;
    std::cout << "Result State (KingCaptureScenario):\n" << result.boardString() << std::endl;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}