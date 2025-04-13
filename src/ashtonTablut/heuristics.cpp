// heuristics.cpp

#include "tablut/heuristics.h"

int8_t Heuristics::getHeuristics(const State& state, const Turn& turn) {
    // 
    if (state.getTurn() == Turn::Draw)
        return -10;
    if (state.getTurn() == Turn::BlackWin) {
        if (turn == Turn::Black) return Heuristics::max;
        else return Heuristics::min;
    }
    if (state.getTurn() == Turn::WhiteWin) {
        if (turn == Turn::White) return Heuristics::max;
        else return Heuristics::min;  
    }

    if (turn == Turn::White) {
        return whiteHeuristics(state);
    } else {
        return blackHeuristics(state);
    }
}

int8_t Heuristics::whiteHeuristics(const State& state) {
    int8_t score = 0;
    for (int i = 0; i < State::size; i++) {
        for (int j = 0; j < State::size; j++) {
            if (state.getPiece(cord(i, j)) == Piece::White)
                score += 5;
            else if (state.getPiece(cord(i, j)) == Piece::Black)
                score -= 5;
            
        }
    }

    return score;
}

int8_t Heuristics::blackHeuristics(const State& state) {
    int8_t score = 0;
    for (int i = 0; i < State::size; i++) {
        for (int j = 0; j < State::size; j++) {
            if (state.getPiece(cord(i, j)) == Piece::Black)
                score += 5;
            else if (state.getPiece(cord(i, j)) == Piece::White)
                score -= 5;
        }
    }

    return score;
}