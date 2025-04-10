#include <gtest/gtest.h>
#include <serverConnection/simpleJson.h>

TEST(SimpleJsonTest, JsonMoveConversion) {
    Move move = {{4, 4}, {5, 5}};
    Turn turn = Turn::White;

    std::string json = SimpleJson::toJson(move, turn);
    std::cout << "Generated JSON: " << json << std::endl;

    // Expected JSON format
    std::string expectedJson = "{\"from\":\"d5\",\"to\":\"e6\",\"turn\":\"WHITE\"}";
    std::cout << "Expected JSON: " << expectedJson << std::endl;
    ASSERT_EQ(json, expectedJson);
}

TEST(SimpleJsonTest, JsonStateConversion) {
    std::string json = "{\"board\":[[\"EMPTY\",\"EMPTY\",\"EMPTY\",\"BLACK\",\"BLACK\",\"BLACK\",\"EMPTY\",\"EMPTY\",\"EMPTY\"],[\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"BLACK\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\"],[\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"WHITE\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\"],[\"BLACK\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"WHITE\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"BLACK\"],[\"BLACK\",\"BLACK\",\"WHITE\",\"WHITE\",\"KING\",\"WHITE\",\"WHITE\",\"BLACK\",\"BLACK\"],[\"BLACK\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"WHITE\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"BLACK\"],[\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"WHITE\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\"],[\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"BLACK\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\"],[\"EMPTY\",\"EMPTY\",\"EMPTY\",\"BLACK\",\"BLACK\",\"BLACK\",\"EMPTY\",\"EMPTY\",\"EMPTY\"]],\"turn\":\"WHITE\"}";
    State state = SimpleJson::fromJson(json);
    std::cout << "Generated State: " << state.boardString() << std::endl;
    State initState;
    ASSERT_EQ(state.softHash(), initState.softHash());
    ASSERT_EQ(state.getTurn(), Turn::White);
}

TEST(SimpleJsonTest, JsonStateConversion2) {
    std::string json = "{\"board\":[[\"EMPTY\",\"BLACK\",\"EMPTY\",\"EMPTY\",\"BLACK\",\"EMPTY\",\"WHITE\",\"EMPTY\",\"BLACK\"],[\"EMPTY\",\"BLACK\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"WHITE\"],[\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"WHITE\",\"EMPTY\",\"EMPTY\",\"EMPTY\"],[\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\"],[\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"THRONE\",\"EMPTY\",\"WHITE\",\"EMPTY\",\"EMPTY\"],[\"EMPTY\",\"EMPTY\",\"BLACK\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"BLACK\",\"EMPTY\"],[\"EMPTY\",\"BLACK\",\"BLACK\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"KING\",\"WHITE\"],[\"EMPTY\",\"EMPTY\",\"EMPTY\",\"BLACK\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"WHITE\",\"BLACK\"],[\"EMPTY\",\"BLACK\",\"EMPTY\",\"EMPTY\",\"EMPTY\",\"BLACK\",\"WHITE\",\"EMPTY\",\"EMPTY\"]],\"turn\":\"WHITE\"}";
    State state = SimpleJson::fromJson(json);
    State expected;
    expected.board[0][0] = Piece::Empty;
    expected.board[0][1] = Piece::Black;
    expected.board[0][2] = Piece::Empty;
    expected.board[0][3] = Piece::Empty;
    expected.board[0][4] = Piece::Black;
    expected.board[0][5] = Piece::Empty;
    expected.board[0][6] = Piece::White;
    expected.board[0][7] = Piece::Empty;
    expected.board[0][8] = Piece::Black;

    expected.board[1][0] = Piece::Empty;
    expected.board[1][1] = Piece::Black;
    expected.board[1][2] = Piece::Empty;
    expected.board[1][3] = Piece::Empty;
    expected.board[1][4] = Piece::Empty;
    expected.board[1][5] = Piece::Empty;
    expected.board[1][6] = Piece::Empty;
    expected.board[1][7] = Piece::Empty;
    expected.board[1][8] = Piece::White;

    expected.board[2][0] = Piece::Empty;
    expected.board[2][1] = Piece::Empty;
    expected.board[2][2] = Piece::Empty;
    expected.board[2][3] = Piece::Empty;
    expected.board[2][4] = Piece::Empty;
    expected.board[2][5] = Piece::White;
    expected.board[2][6] = Piece::Empty;
    expected.board[2][7] = Piece::Empty;
    expected.board[2][8] = Piece::Empty;

    expected.board[3][0] = Piece::Empty;
    expected.board[3][1] = Piece::Empty;
    expected.board[3][2] = Piece::Empty;
    expected.board[3][3] = Piece::Empty;
    expected.board[3][4] = Piece::Empty;
    expected.board[3][5] = Piece::Empty;
    expected.board[3][6] = Piece::Empty;
    expected.board[3][7] = Piece::Empty;
    expected.board[3][8] = Piece::Empty;

    expected.board[4][0] = Piece::Empty;
    expected.board[4][1] = Piece::Empty;
    expected.board[4][2] = Piece::Empty;
    expected.board[4][3] = Piece::Empty;
    expected.board[4][4] = Piece::Empty;
    expected.board[4][5] = Piece::Empty;
    expected.board[4][6] = Piece::White;
    expected.board[4][7] = Piece::Empty;
    expected.board[4][8] = Piece::Empty;

    expected.board[5][0] = Piece::Empty;
    expected.board[5][1] = Piece::Empty;
    expected.board[5][2] = Piece::Black;
    expected.board[5][3] = Piece::Empty;
    expected.board[5][4] = Piece::Empty;
    expected.board[5][5] = Piece::Empty;
    expected.board[5][6] = Piece::Empty;
    expected.board[5][7] = Piece::Black;
    expected.board[5][8] = Piece::Empty;

    expected.board[6][0] = Piece::Empty;
    expected.board[6][1] = Piece::Black;
    expected.board[6][2] = Piece::Black;
    expected.board[6][3] = Piece::Empty;
    expected.board[6][4] = Piece::Empty;
    expected.board[6][5] = Piece::Empty;
    expected.board[6][6] = Piece::Empty;
    expected.board[6][7] = Piece::King;
    expected.board[6][8] = Piece::White;

    expected.board[7][0] = Piece::Empty;
    expected.board[7][1] = Piece::Empty;
    expected.board[7][2] = Piece::Empty;
    expected.board[7][3] = Piece::Black;
    expected.board[7][4] = Piece::Empty;
    expected.board[7][5] = Piece::Empty;
    expected.board[7][6] = Piece::Empty;
    expected.board[7][7] = Piece::White;
    expected.board[7][8] = Piece::Black;

    expected.board[8][0] = Piece::Empty;
    expected.board[8][1] = Piece::Black;
    expected.board[8][2] = Piece::Empty;
    expected.board[8][3] = Piece::Empty;
    expected.board[8][4] = Piece::Empty;
    expected.board[8][5] = Piece::Black;
    expected.board[8][6] = Piece::White;
    expected.board[8][7] = Piece::Empty;
    expected.board[8][8] = Piece::Empty;

    expected.setTurn(Turn::White);
    std::cout << "Generated State: " << state.boardString() << std::endl;
    std::cout << "Expected State: " << expected.boardString() << std::endl;
    ASSERT_EQ(state.softHash(), expected.softHash());
    ASSERT_EQ(state.getTurn(), Turn::White);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}