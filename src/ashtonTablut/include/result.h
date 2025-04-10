// result.h

#ifndef RESULT_H
#define RESULT_H

#include "common.h"
#include "state.h"
#include "actions.h"

class Result {
    public:
        // assumption: the move is valid
        static State applyAction(State s, const Move& m);
};

#endif // RESULT_H