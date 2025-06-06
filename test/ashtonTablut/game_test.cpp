// game_test.cpp

#include <gtest/gtest.h>
#include <functional>
#include <vector>
#include <algorithm>

#include "tablut/game.h"


// Simple utility function for testing purposes
// Returns 1 for WhiteWin, -1 for BlackWin, 0 for Draw, 0 otherwise
int8_t testUtility(State state) {
    switch (state.getTurn()) {
        case Turn::WhiteWin: return 1;
        case Turn::BlackWin: return -1;
        case Turn::Draw: return 0;
        default: return 0; // Utility is typically defined only for terminal states
    }
}

// Test fixture for Game tests
class GameTest : public ::testing::Test {
protected:
    State initialState;
    Game game;

    // Set up the game object before each test
    GameTest() :    initialState(), // Use default constructor for initial state
                    game(initialState,
                        Action::getActions,      // Use actual actions function from actions.h
                        Result::applyAction,     // Use actual result function from result.h
                        Heuristics::getHeuristics,
                        Heuristics::min,
                        Heuristics::max,
                        Heuristics::unknown
                    ) {}

    // Helper to compare vectors of Moves (ignores order)
    bool compareMoveVectors(const std::vector<Move>& v1, const std::vector<Move>& v2) {
        if (v1.size() != v2.size()) {
            return false;
        }
        std::vector<Move> temp_v2 = v2;
        for (const auto& move1 : v1) {
            auto it = std::find(temp_v2.begin(), temp_v2.end(), move1);
            if (it == temp_v2.end()) {
                return false; // Move from v1 not found in v2
            }
            temp_v2.erase(it); // Remove found move to handle duplicates
        }
        return temp_v2.empty(); // If empty, all moves matched
    }
};

// Test getInitialState method
TEST_F(GameTest, GetInitialState) {
    State retrievedState = game.getInitialState();
    // Assuming State has an equals method for comparison
    ASSERT_TRUE(initialState.equals(retrievedState));
}

// Test getPlayers method
TEST_F(GameTest, GetPlayers) {
    std::vector<Turn> players = game.getPlayers();
    ASSERT_EQ(players.size(), 2);
    // Check if both Black and White players are present
    bool foundBlack = std::find(players.begin(), players.end(), Turn::Black) != players.end();
    bool foundWhite = std::find(players.begin(), players.end(), Turn::White) != players.end();
    ASSERT_TRUE(foundBlack);
    ASSERT_TRUE(foundWhite);
}

// Test getActions method
TEST_F(GameTest, GetActions) {
    State currentState; // Initial state
    // Get expected actions directly using the static method
    std::vector<Move> expectedActions = Action::getActions(currentState);
    // Get actions via the Game object
    std::vector<Move> actualActions = game.getActions(currentState);

    // Compare action lists using the helper function (order doesn't matter)
    ASSERT_TRUE(compareMoveVectors(actualActions, expectedActions));
}

// Test getResult method
TEST_F(GameTest, GetResult) {
    State currentState; // Initial state
    std::vector<Move> actions = game.getActions(currentState);
    ASSERT_FALSE(actions.empty()); // Ensure there are actions to test

    Move firstAction = actions[0]; // Pick an action
    // Get expected result directly using the static method
    State expectedState = Result::applyAction(currentState, firstAction);
    // Get result via the Game object
    State actualState = game.getResult(currentState, firstAction);

    // Compare resulting states (assuming State has an equals method)
    ASSERT_TRUE(expectedState.equals(actualState));
}

// Test isTerminal method
TEST_F(GameTest, IsTerminal) {
    State nonTerminalState; // Initial state is non-terminal
    ASSERT_EQ(nonTerminalState.getTurn(), Turn::White); // Verify it's not a terminal turn
    ASSERT_FALSE(game.isTerminal(nonTerminalState));

    // Create terminal states
    State whiteWinState;
    whiteWinState.setTurn(Turn::WhiteWin);
    ASSERT_TRUE(game.isTerminal(whiteWinState));

    State blackWinState;
    blackWinState.setTurn(Turn::BlackWin);
    ASSERT_TRUE(game.isTerminal(blackWinState));

    State drawState;
    drawState.setTurn(Turn::Draw);
    ASSERT_TRUE(game.isTerminal(drawState));
}

// Test getUtility method
TEST_F(GameTest, GetUtility) {
    State terminalState; // Create a base state

    // Test White Win utility
    terminalState.setTurn(Turn::WhiteWin);
    ASSERT_NO_FATAL_FAILURE(game.getUtility(terminalState, Turn::White));

    // Test Black Win utility
    terminalState.setTurn(Turn::BlackWin);
    ASSERT_NO_FATAL_FAILURE(game.getUtility(terminalState, Turn::Black));

    // Test Draw utility
    terminalState.setTurn(Turn::Draw);
    ASSERT_NO_FATAL_FAILURE(game.getUtility(terminalState, Turn::Black));

    // Test utility on a non-terminal state (should return 0 based on testUtility)
    State nonTerminalState;
    nonTerminalState.setTurn(Turn::White); // e.g., White's turn
    ASSERT_NO_FATAL_FAILURE(game.getUtility(nonTerminalState, Turn::White));
}

// Main function for running tests (optional if linking with a main test runner)
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
     return RUN_ALL_TESTS();
}