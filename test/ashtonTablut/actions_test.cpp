#include <gtest/gtest.h>
#include <algorithm>
#include "tablut/actions.h"


// Basic test to check actions in an initial state
TEST(ActionsTest, InitialStateActions) {
    State initialState; // Defaults to initial positions
    // For the initial turn, it should be White or Black
    // Check if there's at least one valid move
    EXPECT_TRUE(Action::isPossibleToMove(initialState));
    auto moves = Action::getActions(initialState);
    EXPECT_FALSE(moves.empty());
}

// Test that no moves are possible if all pieces are removed
TEST(ActionsTest, NoPiecesNoMoves) {
    State s;
    cord c;
    // Remove white pieces so no move are possible for white
    for (int x = 0; x < s.size; x++) {
        for (int y = 0; y < s.size; y++) {
            c = cord(x, y);
            if (s.getPiece(c) == Piece::White || s.getPiece(c) == Piece::King)
                s.removePiece(c);
        }
    }
    EXPECT_FALSE(Action::isPossibleToMove(s));
    auto moves = Action::getActions(s);
    EXPECT_TRUE(moves.empty());

    s = State();
    s.setTurn(Turn::Black);
    // Remove black pieces so no move are possible for black
    for (int x = 0; x < s.size; x++) {
        for (int y = 0; y < s.size; y++) {
            c = cord(x, y);
            if (s.getPiece(c) == Piece::Black)
                s.removePiece(c);
        }
    } 
    EXPECT_FALSE(Action::isPossibleToMove(s));
    moves = Action::getActions(s);
    EXPECT_TRUE(moves.empty());
}

TEST(ActionTest, WhiteAllActions) {
    State s;
    for (int x = 0; x < s.size; x++) {
        for (int y = 0; y < s.size; y++) {
            s.removePiece(cord(x, y));
        }
    }
    s.board[4][4] = Piece::King;
    auto moves = Action::getActions(s);
    cord start = cord(4, 4);
    std::vector<Move> expectedMoves;
    expectedMoves.push_back(Move(start, cord(4, 3))); // Up
    expectedMoves.push_back(Move(start, cord(4, 2)));
    expectedMoves.push_back(Move(start, cord(4, 5))); // Down
    expectedMoves.push_back(Move(start, cord(4, 6)));
    expectedMoves.push_back(Move(start, cord(3, 4))); // Left
    expectedMoves.push_back(Move(start, cord(2, 4)));
    expectedMoves.push_back(Move(start, cord(5, 4))); // Right
    expectedMoves.push_back(Move(start, cord(6, 4)));
    // Check if the moves are the same
    std::cout << "Moves: " << moves.size() << std::endl;
    std::cout << "Expected: " << expectedMoves.size() << std::endl;
    for (size_t i = 0; i < expectedMoves.size(); i++) {
        std::cout << "Expected: " << expectedMoves[i].toString() << std::endl;
    }
    for (size_t i = 0; i < moves.size(); i++) {
        std::cout << "Move: " << moves[i].toString() << std::endl;
    }
    EXPECT_EQ(moves.size(), expectedMoves.size());
    for (size_t i = 0; i < expectedMoves.size(); i++) {
        EXPECT_TRUE(std::find(moves.begin(), moves.end(), expectedMoves[i]) != moves.end());
    }

    // Now I put a piece in the way
    s.board[4][5] = Piece::Black; // remember that the board is inverted
    moves = Action::getActions(s);
    expectedMoves.pop_back(); // Remove the last move
    expectedMoves.pop_back(); // Remove the last move

    std::cout << "Moves: " << moves.size() << std::endl;
    std::cout << "Expected: " << expectedMoves.size() << std::endl;
    for (size_t i = 0; i < expectedMoves.size(); i++) {
        std::cout << "Expected: " << expectedMoves[i].toString() << std::endl;
    }
    for (size_t i = 0; i < moves.size(); i++) {
        std::cout << "Move: " << moves[i].toString() << std::endl;
    }

    EXPECT_EQ(moves.size(), expectedMoves.size());
    for (size_t i = 0; i < expectedMoves.size(); i++) {
        EXPECT_TRUE(std::find(moves.begin(), moves.end(), expectedMoves[i]) != moves.end());
    }

}

TEST(ActionTest, BlackAllMoves) {
    State s = State();
    for (int x = 0; x < s.size; x++) {
        for (int y = 0; y < s.size; y++) {
            s.removePiece(cord(x, y));
        }
    }
    s.setTurn(Turn::Black);
    s.board[0][2] = Piece::White;
    s.board[0][6] = Piece::White;
    s.board[1][3] = Piece::White;
    s.board[1][5] = Piece::White;
    s.board[2][4] = Piece::White;
    s.board[3][1] = Piece::White;
    s.board[0][4] = Piece::Black;
    auto moves = Action::getActions(s);
    std::vector<Move> expectedMoves;
    expectedMoves.push_back(Move(cord(4, 0), cord(3, 0)));
    expectedMoves.push_back(Move(cord(4, 0), cord(5, 0)));
    expectedMoves.push_back(Move(cord(4, 0), cord(4, 1)));

    EXPECT_EQ(moves.size(), expectedMoves.size());
    for (size_t i = 0; i < expectedMoves.size(); i++) {
        EXPECT_TRUE(std::find(moves.begin(), moves.end(), expectedMoves[i]) != moves.end());
    }

    s.board[0][4] = Piece::Empty;
    s.board[0][3] = Piece::Black;
    s.board[0][5] = Piece::Black;
    moves = Action::getActions(s);
    expectedMoves.clear();
    expectedMoves.push_back(Move(cord(3, 0), cord(4, 0)));
    expectedMoves.push_back(Move(cord(5, 0), cord(4, 0)));

    std::cout << "Moves: " << moves.size() << std::endl;
    std::cout << "Expected: " << expectedMoves.size() << std::endl;
    for (size_t i = 0; i < expectedMoves.size(); i++) {
        std::cout << "Expected: " << expectedMoves[i].toString() << std::endl;
    }
    for (size_t i = 0; i < moves.size(); i++) {
        std::cout << "Move: " << moves[i].toString() << std::endl;
    }

    EXPECT_EQ(moves.size(), expectedMoves.size());
    for (size_t i = 0; i < expectedMoves.size(); i++) {
        EXPECT_TRUE(std::find(moves.begin(), moves.end(), expectedMoves[i]) != moves.end());
    }
}

TEST(ActionTest, initialMovesWhite) {
    State s = State();
    s.setTurn(Turn::White);
    std::vector<Move> moves = Action::getActions(s);
    std::cout << "Moves: " << moves.size() << std::endl;
    for (size_t i = 0; i < moves.size(); i++) {
        std::cout << "Move: " << moves[i].toString() << std::endl;
    }
    EXPECT_EQ(moves.size(), 56);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}