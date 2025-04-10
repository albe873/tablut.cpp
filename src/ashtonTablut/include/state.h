// state.h

#ifndef STATE_H
#define STATE_H

#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <algorithm>

#include "common.h"

enum class Piece : int8_t {
    Empty = 0,
    Black = 1,
    White = 2,
    King = 3
};

inline std::string toString(Piece piece);

enum class Turn : int8_t {
    Black = 0,
    White = 1,
    BlackWin = 2,
    WhiteWin = 3,
    Draw = 4
};

inline std::string toString(Turn turn);


class State {
public:
    static const int8_t size = 9;
    Piece board[size][size];
    Turn turn;
    std::vector<int> hashHistory;

    // Static constants
    static const std::vector<cord> whitePieces;
    static const cord throne;
    static const bool campsMask[size][size];

    // Static methods
    static bool isThrone(cord c);
    static bool isCamp(cord c);

    // Constructor
    State();
    State(const Piece (&board)[size][size], Turn turn);
    State(const Piece (&board)[size][size], Turn turn, std::vector<int> hashHistory);

    // Getters
    const Piece (&getBoard() const)[size][size];
    Turn getTurn() const;
    void setTurn(Turn newTurn);

    // pieces
    void removePiece(cord c);
    void movePiece(cord from, cord to);
    //void setPiece(cord c, Piece piece);
    Piece getPiece(cord c) const;


    // Utilities
    bool isEmpty(cord c);
    
    // State History
    bool isHistoryRepeated();
    void clearHistory();
    
    // Print Utilities
    std::string boardString() const;

    State clone() const;
    bool isEqual(const State& other) const;
    int softHash() const;
    int hash() const;
};

#endif // STATE_H