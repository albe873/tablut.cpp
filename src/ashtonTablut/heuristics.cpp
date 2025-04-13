// heuristics.cpp

#include "tablut/heuristics.h"

int8_t Heuristics::getHeuristics(const State& state, const Turn& turn) {
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
    if (state.getTurn() == Turn::Draw)
        return 10;
    int8_t score = 0;
    score += state.getWhitePieces() * 4;
    score -= state.getBlackPieces() * 4;

    return score;
}

int8_t Heuristics::blackHeuristics(const State& state) {
    if (state.getTurn() == Turn::Draw)
        return -10;
    int8_t score = 0;
    score += state.getBlackPieces() * 4;
    score -= state.getWhitePieces() * 4;

    return score;
}