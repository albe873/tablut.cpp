// actions.h

#include "common.h"
#include "state.h"

#ifndef ACTIONS_H
#define ACTIONS_H

class Action {
private:
    static inline bool checksIfValid(const cord& start, const cord& dest, Piece piece, const State& s);

public:
    static std::vector<Move> getActions(const State& s);
    static bool isPossibleToMove(const State& s);
};

#endif // ACTIONS_H