#include <adversarialSearch/ite_minmax_ptt.h>
#include <adversarialSearch/ite_minmax_p.h>
#include <algorithm>
#include <vector>
#include <iostream>

using namespace std;

template <typename S, typename A, typename P, typename U>
class CustomSearch2 : public ite_minmax_p<S, A, P, U> {
protected:

    // Penalize/reward terminal states based on depth
    U evalTerminal(const S& state, const P& player, const int& distance) override {
        auto value = this->game.getUtility(state, player);
        if (value == this->game.util_max)
            return value - distance;
        else if (value == this->game.util_min)
            return value + distance;
        else {
            this->hEvalUsed = true;
            return value;
        }
    }

    // Ordering actions based on heuristic values
    vector<A> orderActions(const S& state, vector<A> actions, const P& player, const int& depth) override {
        if (actions.size() <= 1 || depth < 2)   // no brother ordering if depth is low
            return actions;

        // vector with pairs: action and heuristic value
        vector<pair<A, U>> actionValues;
        actionValues.reserve(actions.size());

        // populate the vector
        for (const auto& action : actions) {
            S newState = this->game.getResult(state, action);
            U heuristicValue = this->game.getUtility(newState, player);
            actionValues.push_back({action, heuristicValue});
        }

        // sort the vector based on heuristic values and player
        // if player == this->game.getPlayer(state), sort in descending order
        // else sort in ascending order
        if (player == this->game.getPlayer(state)) {
            std::sort(actionValues.begin(), actionValues.end(),
                      [](const pair<A, U>& a, const pair<A, U>& b) {return a.second > b.second;}
                     );
        } else {
            std::sort(actionValues.begin(), actionValues.end(),
                      [](const pair<A, U>& a, const pair<A, U>& b) {return a.second < b.second;}
                     );
        }

        // create a new vector with sorted actions
        vector<A> sortedActions;
        sortedActions.reserve(actionValues.size());
        for (const auto& pair : actionValues) {
            sortedActions.push_back(pair.first);
        }

        return sortedActions;
    }

public:
    CustomSearch2(const VGame<S, A, P, U>& game, int startDepth, int maxTimeSeconds)
        : ite_minmax_p<S, A, P, U>(game, startDepth, maxTimeSeconds) {}
};