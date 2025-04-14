// heuristic.h

#include "state.h"
#include "common.h"

class Heuristics {
public:
    static const int8_t max;
    static const int8_t min;

    static int8_t whiteHeuristics(const State&);
    static int8_t blackHeuristics(const State&);
    static int8_t kingEscapeRoutes(const State&);
    static int8_t kingSurrounding(const State&);


    static int8_t getHeuristics(const State&, const Turn&);
};