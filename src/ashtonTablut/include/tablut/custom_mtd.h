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

    // Safe whinner to match evalTerminal
    bool hasSafeWinner(const U& resultUtility, int depth) override {
        return resultUtility <= this->game.util_min || resultUtility >= (this->game.util_max - depth);
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




    pair<A, U> makeDecision(S state) override {
        this->metrics.reset();
        this->table.clear();
        this->timer.start();

        this->currentDepthLimit = this->startDepthLimit;

        auto player = this->game.getPlayer(state);

        // get actions and put them in results array 
        auto actions = orderActions(state, this->game.getActions(state), player, this->currentDepthLimit, 0);
        vector<actionUtility<A, U>> results;

        for (auto action : actions)
            results.push_back({action, this->game.util_min});

        U best_util = this->game.util_min;
        U first_guess = this->eval(state, player); // Initial guess for MTD(f)

        // Iterative Deepening Loop
        do {
            this->incrementDepthLimit();
            this->hEvalUsed = false;

            int maxI = 0;
            #pragma omp parallel for schedule(dynamic, 1)
            for (int i = 0; i < results.size(); i++) {

                auto& actUtil = results[i];
                auto new_state = this->game.getResult(state, actUtil.action);

                // Use the utility from the previous depth for this specific action as a guess,
                // falling back to the overall best guess if it's the first action or unavailable.
                U guess;
                if (this->currentDepthLimit > this->startDepthLimit + 1)
                    guess = actUtil.utility;
                else
                    guess = first_guess;

                auto value = this->mtdfSearch(new_state, player, guess, this->currentDepthLimit - 1);

                // If the search is not timed out, update the maximum index for the results vector
                if (!this->timer.isTimeOut()) {
                    #pragma omp critical
                    {
                        maxI = max(i, maxI);
                    }
                    // it's a reference, update only if in time
                    actUtil.utility = value;
                    actUtil.completed = true;
                }
                else
                    actUtil.completed = false; // incomplete 

                // Update the new results vector with the action and its utility
            }
            results.resize(maxI + 1);

            // Sort the results and update only if the timer is not timed out
            // or if the first two results are completed
            // else use the previous results
            if (!this->timer.isTimeOut() ||
                (results.size() > 1 && results[0].completed && results[1].completed))
            {
                std::sort(results.begin(), results.end());

                // Remove actions with utility significantly lower than the best utility
                auto best_util = results[0].utility;
                int i = 0;
                for (i = 0; i < results.size(); i++) {
                    if (results[i].utility < best_util - 50)
                        break;
                }
                results.resize(i);
            }

            // Print the results for debugging purposes
            //cout << "Depth: " << this->currentDepthLimit << endl;
            //for (const auto& result : results) {
            //    cout << "Action: " << result.action.toString() << ", Utility: " << result.utility << endl;
            //}

            if (!results.empty()) {
                // save best utility
                best_util = results[0].utility;
                // use the best utility for the next guess
                first_guess = best_util;
                
                if (this->hasSafeWinner(best_util, this->currentDepthLimit))
                    break;
                if (results.size() > 1 && this->isSignificantlyBetter(results[0].utility, results[1].utility))
                    break;
                if (results.size() == 1)
                    break;
            }

        } while (!this->timer.isTimeOut() && this->hEvalUsed);

        return {results[0].action, results[0].utility};
    }







    
};