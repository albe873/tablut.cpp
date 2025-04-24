// state.h

#ifndef STATE_H
#define STATE_H

#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <random>

#include "common.h"

enum class Piece {
    Empty = 0,
    Black = 1,
    White = 2,
    King = 3
};

inline std::string toString(Piece piece);

enum class Turn {
    Black = 0,
    White = 1,
    BlackWin = 2,
    WhiteWin = 3,
    Draw = 4
};

inline std::string toString(Turn turn);


class State {
private:
    int whiteP;
    int blackP;
    cord kingPos;

    // Zobrist hashing
    long hash_value;
    static long zobrist_table[9][9][4];
    static long zobrist_turn[5];
    void initZobrist();
    void calculateZobrist();
    void updateZobristPiece(const cord& c, const Piece& piece);
    void updateZobristTurn(const Turn& oldTurn, const Turn& newTurn);

public:
    static const int size = 9;
    Piece board[size][size];
    Turn turn;
    std::vector<int> hashHistory;

    // Static constants
    static const std::vector<cord> whitePieces;
    static const cord throne;
    static const bool campsMask[size][size];

    // Static methods
    static bool isThrone(const cord& c);
    static bool isCamp(const cord& c);

    // Constructor
    State();
    State(const Piece (&board)[size][size], Turn turn);
    State(const Piece (&board)[size][size], Turn turn, std::vector<int> hashHistory);

    // Getters
    const Piece (&getBoard() const)[size][size];
    Turn getTurn() const;
    void setTurn(Turn newTurn);

    // pieces
    void removePiece(const cord& c);
    void movePiece(const cord& from, const cord& to);
    //void setPiece(cord c, Piece piece);
    Piece getPiece(const cord& c) const;

    // Utilities
    bool isEmpty(const cord& c) const;

    // Heuristics Utilities
    int getWhitePieces() const;
    int getBlackPieces() const;
    cord getKingPosition() const;
    
    // State History
    bool isHistoryRepeated();
    void clearHistory();
    void setHistory(std::vector<int> history);
    std::vector<int> getHistory() const;
    void recalculateZobrist();
    
    // Print Utilities
    std::string boardString() const;

    bool equals(const State& other) const;
    int softHash() const;
    long hash() const;
};

#endif // STATE_H