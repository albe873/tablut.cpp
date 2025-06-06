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
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
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
    kingPos = throne;

    // Set the number of pieces
    whiteP = 8;
    blackP = 16;

    // Initialize Zobrist hashing
    initZobrist();
    calculateZobrist();

    hashHistory = std::vector<int>();
    hashHistory.push_back(softHash());
}

State::State(const Piece (&board)[size][size], Turn turn, std::vector<int> hashHistory) {
    this->turn = turn;
    std::memcpy(this->board, board, sizeof(this->board));
    this->hashHistory = hashHistory;
    // Set the number of pieces
    whiteP = 0;
    blackP = 0;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (board[y][x] == Piece::White)
                whiteP++;
            else if (board[y][x] == Piece::Black)
                blackP++;
            else if (board[y][x] == Piece::King)
                kingPos = {x, y};
        }
    }

    // Initialize Zobrist hashing
    initZobrist();
    calculateZobrist();
}

State::State(const Piece (&board)[size][size], Turn turn) 
    : State(board, turn, std::vector<int>()) {}



// ------ Zobrist hashing ------

long State::zobrist_table[State::size][State::size][4];
long State::zobrist_turn[5];

inline void State::initZobrist() {
    static bool initialized = false;    // remain between calls
    if (initialized)
        return;
    initialized = true;


    // Use two 64-bit random numbers to form a 128-bit value
    std::mt19937_64 rng(0xAAAAAAAAAAAAAAAAULL);

    // board
    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++)
            for (int p = 0; p < 4; p++)
                zobrist_table[y][x][p] = rng();
    // turn
    for (int i = 0; i < 5; i++)
        zobrist_turn[i] = rng();

}

inline void State::calculateZobrist() {
    hash_value = 0;
    // board
    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++)
            if (board[y][x] != Piece::Empty)
                hash_value ^= zobrist_table[y][x][static_cast<int>(board[y][x])];
    // turn
    hash_value ^= zobrist_turn[static_cast<int>(turn)];
}

inline void State::updateZobristPiece(const cord& c, const Piece& piece) {
    hash_value ^= zobrist_table[c.y][c.x][static_cast<int>(piece)];
}
inline void State::updateZobristTurn(const Turn& oldTurn, const Turn& newTurn) {
    hash_value ^= zobrist_turn[static_cast<int>(oldTurn)];
    hash_value ^= zobrist_turn[static_cast<int>(newTurn)];
}

long State::hash() const {
    return hash_value;
}


// ------ Interact functions ------

const Piece (&State::getBoard() const)[9][9] {
    return board;
}
Turn State::getTurn() const {
    return turn;
}

void State::setTurn(Turn newTurn) {
    updateZobristTurn(turn, newTurn);
    turn = newTurn;
}

void State::removePiece(const cord& c) {
    Piece toRemove = board[c.y][c.x];
    // update pieces count
    if (toRemove == Piece::White)
        whiteP--;
    else if (toRemove == Piece::Black)
        blackP--;
    
    // remove the piece
    board[c.y][c.x] = Piece::Empty;
    updateZobristPiece(c, toRemove);

    // clear history, the same state cannot be repeated
    clearHistory();
}
void State::movePiece(const cord& from, const cord& to) {
    Piece toMove = board[from.y][from.x];
    board[to.y][to.x] = toMove;
    
    if (toMove == Piece::King)  // update king position
        kingPos = to;
    
    board[from.y][from.x] = Piece::Empty;
    
    // update zobrish hash
    updateZobristPiece(from, toMove);
    updateZobristPiece(to, toMove);

    // update history
    hashHistory.push_back(softHash());
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

cord State::getKingPosition() const {
    return kingPos;
}


// ------ History ------

bool State::isHistoryRepeated() {
    int hash = softHash();
    const int end = hashHistory.size() - 1;
    for (int i = 0; i < end; i++) {
        if (hashHistory[i] == hash)
            return true;
    }
    return false;  
}
void State::clearHistory() {
    hashHistory.clear();
}
void State::setHistory(std::vector<int> history) {
    hashHistory = history;
}
std::vector<int> State::getHistory() const {
    return hashHistory;
}

void State::recalculateZobrist() {
    calculateZobrist();
}



// ------ Utilities ------

// Utilities for Heuristics
int State::getWhitePieces() const {
    return whiteP;
}
int State::getBlackPieces() const {
    return blackP;
}

// Utility to print the board
std::string State::boardString() const {
    std::string result;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
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
    for (int i = 0; i < this->hashHistory.size(); i++) {
        if (this->hashHistory[i] != other.hashHistory[i]) {
            return false;
        }
    }

    return true;
}

// Hash only the board, to use for History
int State::softHash() const {
    return (int)(hash_value);
}
