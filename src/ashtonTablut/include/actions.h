// actions.h

#ifndef ACTIONS_H
#define ACTIONS_H

#include "common.h"
#include "state.h"

class Action {
private:
    static inline bool checksIfValid(cord start, cord dest, Piece piece, State s);

public:
    static std::vector<Move> getActions(State s);
    static bool isPossibleToMove(State s);
};

#endif // ACTIONS_H