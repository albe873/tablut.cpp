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

void State::setPiece(int x, int y, Piece piece) {
    if (x >= 0 && x < size && y >= 0 && y < size) {
        board[y][x] = piece;
    }
}

Piece State::getPiece(int x, int y) const {
    if (x >= 0 && x < size && y >= 0 && y < size) {
        return board[y][x];
    }
    return (Piece) -1; // Return -1 for invalid positions
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

