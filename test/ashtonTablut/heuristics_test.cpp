// heuristics_test.cpp

#include <gtest/gtest.h>
#include "tablut/heuristics.h"
#include "tablut/state.h"

/*

TEST(HeuristicsTest, WhiteHeuristicsBasic) {
    State state;
    // White is default turn
    int8_t score = Heuristics::whiteHeuristics(state);
    EXPECT_NE(score, Heuristics::min);
}

TEST(HeuristicsTest, KingSurroundingInitialState) {
    State state;
    // On initial board, king is on throne with no black pieces around
    int8_t surrounding = Heuristics::kingSurrounding(state);
    EXPECT_EQ(surrounding, 0);
}

TEST(HeuristicsTest, KingSurroundingWithBlackPieces) {
    State state;
    
    int8_t surrounding = Heuristics::kingSurrounding(state);
    EXPECT_EQ(surrounding, 0);

    state.movePiece({4, 0}, {4, 3});
    surrounding = Heuristics::kingSurrounding(state);
    EXPECT_EQ(surrounding, 1);

    state.movePiece({0, 4}, {3, 4});
    surrounding = Heuristics::kingSurrounding(state);
    EXPECT_EQ(surrounding, 2);

    state.movePiece({8, 4}, {5, 4});
    surrounding = Heuristics::kingSurrounding(state);
    EXPECT_EQ(surrounding, 3);
    
    state.movePiece({4, 8}, {4, 5});
    std::cout << state.boardString() << std::endl;
    surrounding = Heuristics::kingSurrounding(state);
    EXPECT_EQ(surrounding, 4);
}

TEST(HeuristicsTest, KingEscapeRoutesInCenter) {
    State state;
    int8_t escapeRoutes = Heuristics::kingEscapeRoutes(state);
    EXPECT_EQ(escapeRoutes, 0);
}

TEST(HeuristicsTest, KingEscapeRoutes) {
    State state;
    state.movePiece({4, 4}, {4, 2});
    state.movePiece({4, 0}, {5, 2});

    int8_t escapes = Heuristics::kingEscapeRoutes(state);
    EXPECT_EQ(escapes, 1);

    state.movePiece({5, 2}, {4, 4});
    escapes = Heuristics::kingEscapeRoutes(state);
    EXPECT_EQ(escapes, 2);
}

TEST(HeuristicsTest, WhiteWinHeuristics) {
    State state;
    state.setTurn(Turn::WhiteWin);
    EXPECT_EQ(Heuristics::getHeuristics(state, Turn::White), Heuristics::max);
    EXPECT_EQ(Heuristics::getHeuristics(state, Turn::Black), Heuristics::min);
}

TEST(HeuristicsTest, DrawHeuristics) {
    State state;
    state.setTurn(Turn::Draw);
    EXPECT_EQ(Heuristics::whiteHeuristics(state), 10);
    EXPECT_EQ(Heuristics::blackHeuristics(state), -10);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

    
*/