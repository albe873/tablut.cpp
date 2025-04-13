// heuristic.h

#include "state.h"
#include "common.h"

class Heuristics {
private:
    static int8_t whiteHeuristics(const State&);
    static int8_t blackHeuristics(const State&);
public:
    static int8_t getHeuristics(const State&, const Turn&);
    static const int8_t max = 100;
    static const int8_t min = -100;
};