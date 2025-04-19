// heuristic.h

#include "state.h"
#include "common.h"

#ifndef HEURISTIC_H
#define HEURISTIC_H

class Heuristics {
private:

    static int whiteHeuristics(const State&);
    static int blackHeuristics(const State&);
    static int kingEscapeRoutes(const State&);
    static int kingSurrounding(const State&);
    static int drawLogic(const State&, const Turn&);
    static bool kingNearThrone(const State&);
    static int whiteInBestPositions(const State&);
    static int blackInBestPositions(const State&);

    // weights and constants
    static const std::vector<cord> bestPositionsWhite;
    static const std::vector<cord> bestPositionsBlack;
    static const int whiteDraw;
    static const int blackDraw;

public:
    static const int max;
    static const int min;
    static const int unknown;
    static const bool nearThroneMask[State::size][State::size];

    static int getHeuristics(const State&, const Turn&);
};

#endif // HEURISTIC_H