#include <gtest/gtest.h>
#include "actions.h"

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
    State emptyState;
    cord c;
    // Remove key pieces so no one can move
    for (int x = 0; x < emptyState.size; x++) {
        for (int y = 0; y < emptyState.size; y++) {
            c = cord(x, y);
            emptyState.removePiece(c);
        }
    }
    EXPECT_FALSE(Action::isPossibleToMove(emptyState));
    auto moves = Action::getActions(emptyState);
    EXPECT_TRUE(moves.empty());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}