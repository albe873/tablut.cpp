// actions.h

#ifndef ACTIONS_H
#define ACTIONS_H

#include "common.h"
#include "state.h"

class Action {
private:
    inline bool isEmpty(cord c, State s);
    inline bool isThrone(cord c, State s);
    inline bool isCamp(cord c, State s);
    inline bool checksIfValid(cord start, cord dest, Piece piece, State s);

public:
    std::vector<Move> getActions(State s);
};

#endif // ACTIONS_H