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
private:
    int8_t whiteP;
    int8_t blackP;
    cord kingPos;
public:
    static const int8_t size = 9;
    Piece board[size][size];
    Turn turn;
    std::vector<int16_t> hashHistory;

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
    State(const Piece (&board)[size][size], Turn turn, std::vector<int16_t> hashHistory);

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
    int8_t getWhitePieces() const;
    int8_t getBlackPieces() const;
    cord getKingPosition() const;
    
    // State History
    bool isHistoryRepeated();
    void clearHistory();
    void setHistory(std::vector<int16_t> history);
    std::vector<int16_t> getHistory() const;
    
    // Print Utilities
    std::string boardString() const;

    State clone() const;
    bool equals(const State& other) const;
    int16_t softHash() const;
    int hash() const;
};

#endif // STATE_H