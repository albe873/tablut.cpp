#include <gtest/gtest.h>
#include <tablut/state.h>

TEST(StateTest, InitialState) {
    std::cout << "Initial state test" << std::endl;
    State state = State();
    std::string expected = 
     R"(- - - B B B - - - 
- - - - B - - - - 
- - - - W - - - - 
B - - - W - - - B 
B B W W K W W B B 
B - - - W - - - B 
- - - - W - - - - 
- - - - B - - - - 
- - - B B B - - - 
)";

    std::cout << "state board toString\n" << std::endl;
    std::cout << state.boardString() << std::endl;
    std::cout << "expected board toString\n" << std::endl;
    std::cout << expected << std::endl;
    ASSERT_TRUE(expected.compare(state.boardString()) == 0);
}

TEST(StateTest, RemovePieceTest) {
    std::cout << "RemovePiece test" << std::endl;
    State state;
    // Remove a piece at the center (King's position)
    cord kingPos{4,4};
    ASSERT_EQ(state.getPiece(kingPos), Piece::King);
    state.removePiece(kingPos);
    ASSERT_EQ(state.getPiece(kingPos), Piece::Empty);
}

TEST(StateTest, MovePieceTest) {
    std::cout << "MovePiece test" << std::endl;
    State state;
    cord from{3,0}; // Example of a black piece position
    cord to{2,0};
    ASSERT_EQ(state.getPiece(from), Piece::Black);
    ASSERT_EQ(state.getPiece(to), Piece::Empty);

    state.movePiece(from, to);

    ASSERT_EQ(state.getPiece(from), Piece::Empty);
    ASSERT_EQ(state.getPiece(to), Piece::Black);
}

TEST(StateTest, GetPieceAndIsEmptyTest) {
    std::cout << "GetPiece and IsEmpty test" << std::endl;
    State state;
    cord blackPos{0,3};
    ASSERT_EQ(state.getPiece(blackPos), Piece::Black);
    ASSERT_FALSE(state.isEmpty(blackPos));

    cord emptyPos{0,0};
    ASSERT_EQ(state.getPiece(emptyPos), Piece::Empty);
    ASSERT_TRUE(state.isEmpty(emptyPos));
}

TEST(StateTest, HistoryRepeatedTest) {
    std::cout << "HistoryRepeated test" << std::endl;
    State state;

    // Make a move then revert it to get same position
    cord from{3,0}; 
    cord to{2,0};
    std::cout << "Initial hash history: ";
    for (const auto& hash : state.hashHistory) {
        std::cout << hash << " ";
    }
    std::cout << std::endl;
    state.movePiece(from, to);
    ASSERT_FALSE(state.isHistoryRepeated());
    std::cout << "After move hash history: ";
    for (const auto& hash : state.hashHistory) {
        std::cout << hash << " ";
    }
    std::cout << std::endl;
    state.movePiece(to, from);
    std::cout << "After revert hash history: ";
    for (const auto& hash : state.hashHistory) {
        std::cout << hash << " ";
    }
    std::cout << std::endl;

    ASSERT_TRUE(state.isHistoryRepeated()); 
}

TEST(StateTest, HistoryNotRepeatedTest) {
    std::cout << "History not repeated test" << std::endl;
    State state;
    cord from{3,0};
    cord to{2,0};

    // Make a single move without returning to a previous position
    state.movePiece(from, to);

    from = {2,0};
    to = {1,0};

    state.movePiece(from, to);
    
    // Check that history is not repeated
    ASSERT_FALSE(state.isHistoryRepeated());
}

TEST(StateTest, ClearHistoryTest) {
    std::cout << "ClearHistory test" << std::endl;
    State state;
    state.hashHistory.push_back(123);
    ASSERT_FALSE(state.hashHistory.empty());

    state.clearHistory();
    ASSERT_TRUE(state.hashHistory.empty());
}

TEST(StateTest, CloneAndIsEqualTest) {
    std::cout << "Clone and isEqual test" << std::endl;
    State state;
    state.removePiece(cord(4, 4));
    State cloned = state.clone();

    ASSERT_TRUE(state.equals(cloned));
    ASSERT_TRUE(state.getPiece(cord(4, 4)) == Piece::Empty);
    ASSERT_TRUE(cloned.getPiece(cord(4, 4)) == Piece::Empty);

    // Change something in the clone
    cloned.removePiece(cord(3, 4));
    ASSERT_FALSE(state.equals(cloned));
}

TEST(StateTest, CloneIndependenceTest) {
    std::cout << "Clone independence test" << std::endl;
    State state;
    // Store a piece before cloning
    Piece originalPiece = state.getPiece({4,4});

    // Clone the state
    State cloned = state.clone();

    // Modify the cloned state
    cloned.removePiece({4,4});

    // Verify original is unaffected
    ASSERT_EQ(state.getPiece({4,4}), originalPiece);
}

TEST(StateTest, CloneTurnIndependenceTest) {
    std::cout << "Clone turn independence test" << std::endl;
    State state;
    // Store the current turn before cloning
    Turn originalTurn = state.getTurn();

    // Clone the state
    State cloned = state.clone();

    // Modify the cloned state's turn
    cloned.setTurn(Turn::Black);

    // Verify original state's turn is unaffected
    ASSERT_EQ(state.getTurn(), originalTurn);
}

TEST(StateTest, CloneHistoryIndependenceTest) {
    std::cout << "Clone history independence test" << std::endl;
    State state;
    // Add an entry to the history before cloning
    state.hashHistory.push_back(123);

    // Clone the state
    State cloned = state.clone();

    // Modify the cloned state's history
    cloned.hashHistory.push_back(456);

    // Verify original state's history is unaffected
    ASSERT_EQ(state.hashHistory.size(), 2);
    ASSERT_EQ(state.hashHistory.back(), 123);
}

TEST(StateTest, HashFunctionsTest) {
    std::cout << "Hash functions test" << std::endl;
    State state;
    auto softH = state.softHash();
    auto fullH = state.hash();

    // We only test that these return some value and remain consistent
    ASSERT_NE(softH, 0);
    ASSERT_NE(fullH, 0);

    // Move a piece and verify the hash changes
    auto oldSoft = state.softHash();
    state.movePiece({3,0}, {2,0});
    auto newSoft = state.softHash();
    ASSERT_NE(oldSoft, newSoft);
}

TEST(StateTest, PieceCountTest) {
    std::cout << "Piece count test" << std::endl;
    State state;
    ASSERT_EQ(state.getWhitePieces(), 8);
    ASSERT_EQ(state.getBlackPieces(), 16);

    // Remove pieces and verify the count changes
    state.removePiece({4,4});
    ASSERT_EQ(state.getWhitePieces(), 8);
    ASSERT_EQ(state.getBlackPieces(), 16);

    state.removePiece({0,3});
    ASSERT_EQ(state.getWhitePieces(), 8);
    ASSERT_EQ(state.getBlackPieces(), 15);
    state.removePiece({3,4});
    ASSERT_EQ(state.getWhitePieces(), 7);
    ASSERT_EQ(state.getBlackPieces(), 15);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}