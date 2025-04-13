// state.cpp

#include "tablut/state.h"

std::string toString(Turn turn) {
    switch (turn) {
        case Turn::Black: return "Black";
        case Turn::White: return "White";
        case Turn::BlackWin: return "Black Win";
        case Turn::WhiteWin: return "White Win";
        case Turn::Draw: return "Draw";
        default: return "Unknown";
    }
}

std::string toString(Piece piece) {
    switch (piece) {
        case Piece::Empty: return "-";
        case Piece::Black: return "B";
        case Piece::White: return "W";
        case Piece::King: return "K";
        default: return "E";
    }
}

// --- Static definitions ---

const bool State::campsMask[size][size] = { 
    {false, false, false, true,  true,  true,  false, false, false},
    {false, false, false, false, true,  false, false, false, false},
    {false, false, false, false, false, false, false, false, false},
    {true,  false, false, false, false, false, false, false, true},
    {true,  true,  false, false, false, false, false, true,  true},
    {true,  false, false, false, false, false, false, false, true},
    {false, false, false, false, false, false, false, false, false},
    {false, false, false, false, true,  false, false, false, false},
    {false, false, false, true,  true,  true,  false, false, false}
};

const cord State::throne = {4, 4}; // King position
const std::vector<cord> State::whitePieces = {
    {4, 2}, {4, 3}, {4, 5}, {4, 6}, // vertical
    {2, 4}, {3, 4}, {5, 4}, {6, 4}  // horizontal
};

// --- Static methods ---
bool State::isThrone(const cord& c) {
    return c == State::throne;
}
bool State::isCamp(const cord& c) {
    return  c.x >= 0 && c.x < size && 
            c.y >= 0 && c.y < size && 
            State::campsMask[c.y][c.x];
}


// ------ Contructors -------

State::State() {
    turn = Turn::White;

    // Initialize the board with pieces
    for (int8_t y = 0; y < size; y++) {
        for (int8_t x = 0; x < size; x++) {
            if (campsMask[y][x]) {
                board[y][x] = Piece::Black;
            }
            else
                board[y][x] = Piece::Empty;
        }
    }

    // White pieces
    for (cord white : whitePieces) {
        board[white.y][white.x] = Piece::White;
    }

    // King
    board[throne.y][throne.x] = Piece::King;
    
    hashHistory = std::vector<int16_t>();
    hashHistory.push_back(softHash());

    // Set the number of pieces
    whiteP = 8;
    blackP = 16;
}

State::State(const Piece (&board)[9][9], Turn turn, std::vector<int16_t> hashHistory) {
    this->turn = turn;
    std::memcpy(this->board, board, sizeof(this->board));
    this->hashHistory = hashHistory;
    // Set the number of pieces
    whiteP = 0;
    blackP = 0;
    for (int8_t y = 0; y < size; y++) {
        for (int8_t x = 0; x < size; x++) {
            if (board[y][x] == Piece::White)
                whiteP++;
            else if (board[y][x] == Piece::Black)
                blackP++;
        }
    }
}
State::State(const Piece (&board)[9][9], Turn turn) 
    : State(board, turn, std::vector<int16_t>()) {}



// ------ Interact functions ------

const Piece (&State::getBoard() const)[9][9] {
    return board;
}
Turn State::getTurn() const {
    return turn;
}

void State::setTurn(Turn newTurn) {
    turn = newTurn;
}

void State::removePiece(const cord& c) {
    // update pieces count
    if (board[c.y][c.x] == Piece::White)
        whiteP--;
    else if (board[c.y][c.x] == Piece::Black)
        blackP--;
    
    // remove the piece
    board[c.y][c.x] = Piece::Empty;

    // clear history, the same state cannot be repeated
    clearHistory();
}
void State::movePiece(const cord& from, const cord& to) {
    board[to.y][to.x] = board[from.y][from.x];
    board[from.y][from.x] = Piece::Empty;
}

Piece State::getPiece(const cord& c) const {
    if (c.x >= 0 && c.x < size && c.y >= 0 && c.y < size) {
        return board[c.y][c.x];
    }
    return (Piece) -1; // Return -1 for invalid positions
}

bool State::isEmpty(const cord& c) const {
    return board[c.y][c.x] == Piece::Empty;
}


// ------ History ------

bool State::isHistoryRepeated() {
    int hash = softHash();
    for (int i = 0; i < hashHistory.size(); i++) {
        if (hashHistory[i] == hash)
            return true;
    }
    // If not found, add to history
    hashHistory.push_back(hash);
    return false;  
}
void State::clearHistory() {
    hashHistory.clear();
}
void State::setHistory(std::vector<int16_t> history) {
    hashHistory = history;
}
std::vector<int16_t> State::getHistory() const {
    return hashHistory;
}



// ------ Utilities ------

// Utilities for Heuristics
int8_t State::getWhitePieces() const {
    return whiteP;
}
int8_t State::getBlackPieces() const {
    return blackP;
}

// Utility to print the board
std::string State::boardString() const {
    std::string result;
    for (int8_t y = 0; y < size; y++) {
        for (int8_t x = 0; x < size; x++) {
            if (isEmpty({x, y}) && (isCamp({x, y}) || isThrone({x, y}))) {
                result += "+ ";
            }
            else
                result += toString(board[y][x]) + ' ';
        }
        result += '\n';
    }
    return result;
}


// Utility to clone the state
State State::clone() const {
    return State(this->board, this->turn, this->hashHistory);
}



// ------ equals and hash functions ------

bool State::equals(const State& other) const {
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

    if (this->hashHistory.size() != other.hashHistory.size()) {
        return false;
    }
    for (size_t i = 0; i < this->hashHistory.size(); i++) {
        if (this->hashHistory[i] != other.hashHistory[i]) {
            return false;
        }
    }

    return true;
}

// Hash only the board, to use for History
int16_t State::softHash() const {
    int hash = 0;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            hash ^= static_cast<int16_t>(board[y][x]) * (x + 1) * (y*8 + 1);
        }
    }
    return hash;
}

int State::hash() const {
    int hash = softHash();
    hash ^= static_cast<int>(turn) * 1024;
    return hash;
}