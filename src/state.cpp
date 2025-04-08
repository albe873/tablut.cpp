// state.cpp

#include "state.h"

inline std::string toString(Turn turn) {
    switch (turn) {
        case Turn::Black: return "Black";
        case Turn::White: return "White";
        case Turn::BlackWin: return "Black Win";
        case Turn::WhiteWin: return "White Win";
        case Turn::Draw: return "Draw";
        default: return "Unknown";
    }
}

inline std::string toString(Piece piece) {
    switch (piece) {
        case Piece::Empty: return "0";
        case Piece::Black: return "B";
        case Piece::White: return "W";
        case Piece::King: return "K";
        default: return "E";
    }
}

// --- Static definitions ---
static const int8_t size = 9;

const std::vector<cord> State::camps = {
    {3, 0}, {4, 0}, {5, 0}, {4, 1}, // camp on top
    {0, 3}, {0, 4}, {0, 5}, {1, 4}, // camp on left
    {8, 3}, {8, 4}, {8, 5}, {7, 4}, // camp on right
    {3, 8}, {4, 8}, {5, 8}, {4, 7}  // camp on bottom
};
const cord State::throne = {4, 4}; // King position
const std::vector<cord> State::whitePieces = {
    {4, 2}, {4, 3}, {4, 5}, {4, 6}, // vertical
    {2, 4}, {3, 4}, {5, 4}, {6, 4}  // horizontal
};
const std::vector<cord> State::escapes = {
    {1, 0}, {2, 0}, {6, 0}, {7, 0}, //escapes on top
    {0, 1}, {0, 2}, {0, 6}, {0, 7}, //escapes on left
    {8, 1}, {8, 2}, {8, 6}, {8, 7}, //escapes on right
    {1, 8}, {2, 8}, {6, 8}, {7, 8}  //escapes on bottom
};

// --- Static methods ---
inline bool State::isThrone(cord c) {
    return c == State::throne;
}
inline bool State::isCamp(cord c) {
    for (cord camp : State::camps) {
        if (c == camp)
            return true;
    }
    return false;
}


// --- Contructors ---

State::State() {
    turn = Turn::White;
    for (int8_t y = 0; y < size; y++) {
        for (int8_t x = 0; x < size; x++) {
            board[y][x] = Piece::Empty;
        }
    }
    // Initialize the board with pieces

    // Black pieces
    for (cord camp : camps) {
        board[camp.y][camp.x] = Piece::Black;
    }

    // White pieces
    for (cord white : whitePieces) {
        board[white.y][white.x] = Piece::White;
    }

    // King
    board[throne.y][throne.x] = Piece::King;
}

State::State(const Piece (&board)[9][9], Turn turn) {
    this->turn = turn;
    std::memcpy(this->board, board, sizeof(this->board));
}

// --- Getters and setters ---

const Piece (&State::getBoard() const)[9][9] {
    return board;
}
Turn State::getTurn() const {
    return turn;
}

void State::setTurn(Turn newTurn) {
    turn = newTurn;
}

void State::setPiece(cord c, Piece piece) {
    if (c.x >= 0 && c.x < size && c.y >= 0 && c.y < size) {
        board[c.y][c.x] = piece;
    }
}


Piece State::getPiece(cord c) const {
    if (c.x >= 0 && c.x < size && c.y >= 0 && c.y < size) {
        return board[c.y][c.x];
    }
    return (Piece) -1; // Return -1 for invalid positions
}

// --- Utilities ---

bool State::isEmpty(cord c) {
    return board[c.y][c.x] == Piece::Empty;
}


// Utility to print the board
void State::printBoard() {
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            std::cout << toString(board[y][x]) << ' ';
        }
        std::cout << '\n';
    }
}

std::string State::boardString() const {
    std::string result;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            result += toString(board[y][x]) + ' ';
        }
        result += '\n';
    }
    return result;
}

// Utility to clone the state
State State::clone() const {
    return State(this->board, this->turn);
}

bool State::isEqual(const State& other) const {
    if (this->turn != other.turn) {
        return false;
    }

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (this->board[y][x] != other.board[y][x]) {
                return false;
            }
        }
    }
    return true;
}

int State::hash() const {
    int hash = 0;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            hash ^= static_cast<int>(board[y][x]) * (x + 1) * (y + 1);
        }
    }
    return hash;
}

