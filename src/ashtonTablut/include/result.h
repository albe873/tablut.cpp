// result.h

#ifndef RESULT_H
#define RESULT_H

#include "common.h"
#include "state.h"

class Result {
    public:
        // assumption: the move is valid
        State applyAction(State s, Move m);
};

#endif // RESULT_H