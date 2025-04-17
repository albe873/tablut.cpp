#include <adversarialSearch/parIteDABSearch.h>
#include <algorithm>
#include <vector>
#include <iostream>

using namespace std;

template <typename S, typename A, typename P, typename U>
class CustomSearch : public parIteDABSearch<S, A, P, U> {
protected:

    // Penalize/reward terminal states based on depth
    U evalTerminal(const S& state, const P& player, const int& depth) override {
        U utility = this->game.getUtility(state, player);
        if (utility == this->utilMax)
            return utility - depth;
        else if (utility == this->utilMin)
            return utility + depth;
        else
            return utility;
    }

    // Ordering actions based on heuristic values
    vector<A> orderActions(const S& state, const vector<A>& actions, const P& player, const int& depth) override {
        if (actions.size() <= 1)
            return actions;

        vector<pair<A, U>> actionValues;
        actionValues.reserve(actions.size());

        for (const auto& action : actions) {
            S newState = this->game.getResult(state, action);
            U heuristicValue = this->game.getUtility(newState, player);
            actionValues.push_back({action, heuristicValue});
        }

        if (player == this->game.getPlayer(state)) {
            std::sort(actionValues.begin(), actionValues.end(),
                      [](const pair<A, U>& a, const pair<A, U>& b) {return a.second > b.second;}
                     );
        } else {
            std::sort(actionValues.begin(), actionValues.end(),
                      [](const pair<A, U>& a, const pair<A, U>& b) {return a.second < b.second;}
                     );
        }

        vector<A> sortedActions;
        sortedActions.reserve(actionValues.size());
        for (const auto& pair : actionValues) {
            sortedActions.push_back(pair.first);
        }

        return sortedActions;
    }

public:
    CustomSearch(const VGame<S, A, P, U>& game, U utilMin, U utilMax, int startDepth, int maxTimeSeconds)
        : parIteDABSearch<S, A, P, U>(game, utilMin, utilMax, startDepth, maxTimeSeconds) {}
};