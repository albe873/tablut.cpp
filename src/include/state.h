// state.h

#ifndef STATE_H
#define STATE_H

#include <vector>
#include <string>
#include <iostream>
#include <cstring>

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
    Piece board[9][9];
    Turn turn;
public:

    const int8_t size = 9;
    const std::vector<cord> camps = {
        {3, 0}, {4, 0}, {5, 0}, {4, 1}, // camp on top
        {0, 3}, {0, 4}, {0, 5}, {1, 4}, // camp on left
        {8, 3}, {8, 4}, {8, 5}, {7, 4}, // camp on right
        {3, 8}, {4, 8}, {5, 8}, {4, 7}  // camp on bottom
    };
    const cord throne = {4, 4}; // King position
    const std::vector<cord> whitePieces = {
        {4, 2}, {4, 3}, {4, 5}, {4, 6}, // vertical
        {2, 4}, {3, 4}, {5, 4}, {6, 4}  // horizontal
    };
    const std::vector<cord> escapes = {
        {1, 0}, {2, 0}, {6, 0}, {7, 0}, //escapes on top
        {0, 1}, {0, 2}, {0, 6}, {0, 7}, //escapes on left
        {8, 1}, {8, 2}, {8, 6}, {8, 7}, //escapes on right
        {1, 8}, {2, 8}, {6, 8}, {7, 8}  //escapes on bottom
    };

    // Constructor
    State();
    State(const Piece (&board)[9][9], Turn turn);

    // Getters
    const Piece (&getBoard() const)[9][9];
    Turn getTurn() const;
    void setTurn(Turn newTurn);
    void setPiece(int x, int y, Piece piece);
    Piece getPiece(int x, int y) const;
    
    // Utilities
    void printBoard();
    std::string boardString() const;

    State clone() const;
    bool isEqual(const State& other) const;
    int hash() const;
};

#endif // STATE_H