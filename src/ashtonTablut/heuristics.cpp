// heuristics.cpp

#include "tablut/heuristics.h"

int8_t Heuristics::getHeuristics(const State& state, const Turn& turn) {
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
            if ((int8_t) state.getPiece(cord(i, j)) == (int8_t) Piece::White)
                score += 1;
            else if ((int8_t) state.getPiece(cord(i, j)) == (int8_t) Piece::Black)
                score -= 1;
            
        }
    }

    return score;
}

int8_t Heuristics::blackHeuristics(const State& state) {
    int8_t score = 0;
    for (int i = 0; i < State::size; i++) {
        for (int j = 0; j < State::size; j++) {
            if ((int8_t) state.getPiece(cord(i, j)) == (int8_t) Piece::Black)
                score += 1;
            else if ((int8_t) state.getPiece(cord(i, j)) == (int8_t) Piece::White)
                score -= 1;
        }
    }

    return score;
}