#include <adversarialSearch/ite_minmax_ptt.h>
#include <adversarialSearch/mtd.h>
#include <algorithm>
#include <vector>
#include <iostream>

using namespace std;

template <typename S, typename A, typename P, typename U>
class custom_mtd : public mtd<S, A, P, U> {
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
    vector<A> orderActions(const S& state, vector<A> actions, const P& player, const int& depth, const int& ba_i) override {
        if (actions.size() <= 1 || depth < 2) {  // no brother ordering if depth is low
            // if valid, put the best action at the beginning
            if (ba_i > 0 && ba_i < actions.size())
                std::swap(actions[0], actions[ba_i]);
            return actions;
        }

        // vector with pairs: action and heuristic value
        vector<pair<A, U>> actions_values;
        actions_values.reserve(actions.size());

        // populate the vector
        for (const auto& action : actions) {
            S newState = this->game.getResult(state, action);
            U heuristicValue = this->game.getUtility(newState, player);
            actions_values.push_back({action, heuristicValue});
        }

        // sort the vector based on heuristic values and player
        // if player == this->game.getPlayer(state), sort in descending order
        // else sort in ascending order
        if (player == this->game.getPlayer(state)) {
            std::sort(actions_values.begin(), actions_values.end(),
                      [](const pair<A, U>& a, const pair<A, U>& b) {return a.second > b.second;}
                     );
        } else {
            std::sort(actions_values.begin(), actions_values.end(),
                      [](const pair<A, U>& a, const pair<A, U>& b) {return a.second < b.second;}
                     );
        }

        // create a new vector with sorted actions
        vector<A> sorted_actions;
        sorted_actions.reserve(actions_values.size());
        for (const auto& pair : actions_values)
            sorted_actions.push_back(pair.first);

        // if valid, put the best action at the beginning
        if (ba_i > 0 && ba_i < actions.size())
            std::swap(sorted_actions[0], sorted_actions[ba_i]);

        return sorted_actions;
    }

public:
    custom_mtd(const VGame<S, A, P, U>& game, int startDepth, int maxTimeSeconds)
        : mtd<S, A, P, U>(game, startDepth, maxTimeSeconds) {}
};