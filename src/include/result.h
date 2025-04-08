// result.h

#ifndef RESULT_H
#define RESULT_H

#include "common.h"
#include "state.h"

class Result {
    public:
        State applyAction(State s, Move m);
};

#endif // RESULT_H