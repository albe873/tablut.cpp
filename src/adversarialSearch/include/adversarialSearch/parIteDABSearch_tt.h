// IntDeepAlphaBetaSearch.h

#include <set>
#include <omp.h>
#include <algorithm>

#include "vgame.h"
#include "utilities.h"
#include "t_table.h"

#ifndef PARITEDEEPABSEARCHTT_H
#define PARITEDEEPABSEARCHTT_H

using namespace std;

/*
pseudo code for the transposition table
https://people.csail.mit.edu/plaat/mtdf.html#abmem

if retrieve(n) == OK then // Transposition table lookup

if n.lowerbound >= beta then return n.lowerbound;
if n.upperbound <= alpha then return n.upperbound;
alpha := max(alpha, n.lowerbound);
beta := min(beta, n.upperbound);

if d == 0 then g := evaluate(n); // leaf node
else if n == MAXNODE then

g := -INFINITY; a := alpha; // save original alpha value
c := firstchild(n);
while (g < beta) and (c != NOCHILD) do
    g := max(g, AlphaBetaWithMemory(c, a, beta, d - 1));
    a := max(a, g);
    c := nextbrother(c);

else // n is a MINNODE

g := +INFINITY; b := beta; // save original beta value 
c := firstchild(n);
while (g > alpha) and (c != NOCHILD) do
    g := min(g, AlphaBetaWithMemory(c, alpha, b, d - 1));
    b := min(b, g);
    c := nextbrother(c);

// Traditional transposition table storing of bounds
// Fail low result implies an upper bound
if g <= alpha then n.upperbound := g; store n.upperbound;
// Found an accurate minimax value - will not occur if called with zero window
if g >  alpha and g < beta then

n.lowerbound := g; n.upperbound := g; store n.lowerbound, n.upperbound;

// Fail high result implies a lower bound
if g >= beta then n.lowerbound := g; store n.lowerbound;
return g;

*/

// Metrics are disabled by default
// they can be enabled by defining ENABLE_METRICS in this file: #define ENABLE_METRICS
// or (preferred way) by passing -DENABLE_METRICS to the compiler

template <typename S, typename A, typename P, typename U>
class parIteDABSearch_tt {
private:
    void inline updateMiss() {
        #ifdef ENABLE_METRICS
            metrics.incrementTTMiss();
        #endif
    }
    void inline updateHit() {
        #ifdef ENABLE_METRICS
            metrics.incrementTTHit();
        #endif
    }
    void updateMetrics(int depth) {
        #ifdef ENABLE_METRICS
            metrics.updateMaxDepth(depth);
            metrics.incrementNodesExpanded();
        #endif
    }

protected:
    const VGame<S, A, P, U>& game;
    bool hEvalUsed;
    int currentDepthLimit;
    Timer timer;
    SimpleMetrics metrics;
    t_table<U, A> table;

    U maxValue(S& state, P& player, U alpha, U beta, int depth) {
        updateMetrics(depth);

        if (game.isTerminal(state)) {
            U value = eval(state, player);
            //table.insert(state.hash64(), entry_type::exact, value, depth);
            return evalTerminal(value, player, depth);
        }

        if (depth >= currentDepthLimit || timer.isTimeOut())
            return eval(state, player);
        
        // Check transposition table
        auto hash = state.hash64();
        auto value = table.probe(hash, alpha, beta, depth);
        if (value != game.util_unknown) {
            updateHit();
            return evalTerminal(value, player, depth);
        }
        updateMiss();
        
        value = game.util_min;

        auto actions = orderActions(state, game.getActions(state), player, depth);
        for (auto action : actions) {
      
            auto newState = game.getResult(state, action);
            value = max(value, minValue(newState, player, alpha, beta, depth + 1));
            
            if (value >= beta) {
                // Insert in transposition table
                table.insert(hash, entry_type::l_bound, value, depth);
                // cutoff
                return value;
            }
            
            // recursive call
            alpha = max(alpha, value);
        }

        return value;
    }


    U minValue(S& state, P& player, U alpha, U beta, int depth) {
        updateMetrics(depth);

        if (game.isTerminal(state)) {
            U value = eval(state, player);
            //table.insert(state.hash64(), entry_type::exact, value, depth);
            return evalTerminal(value, player, depth);
        }
    
        if (depth >= currentDepthLimit || timer.isTimeOut())
            return eval(state, player);
        
        // Check transposition table
        auto hash = state.hash64();
        auto value = table.probe(hash, alpha, beta, depth);
        if (value != game.util_unknown) {
            updateHit();
            return evalTerminal(value, player, depth);
        }
        updateMiss();

        value = game.util_max;
        // save original beta for the transposition table
        U original_beta = beta;
        
        auto actions = orderActions(state, game.getActions(state), player, depth);
        for (auto action : actions) {
            auto newState = game.getResult(state, action);
            value = min(value, maxValue(newState, player, alpha, beta, depth + 1));
            
            if (value <= alpha) {
                table.insert(hash, entry_type::u_bound, value, depth);
                break;
            }

            beta = min(beta, value);
        }

        return value;
    }

    virtual void incrementDepthLimit() {
        this->currentDepthLimit++;
    }

    virtual bool isSignificantlyBetter(const U& newUtility, const U& utility) {
        return false;
    }

    virtual bool hasSafeWinner(const U& resultUtility) {
        if (resultUtility <= game.util_min || resultUtility >= game.util_max)
            std::cout << "Safe winner found: " << resultUtility << std::endl;
        return resultUtility <= game.util_min || resultUtility >= game.util_max;
    }

    virtual U eval(const S& state, const P& player) {
        hEvalUsed = true;
        return game.getUtility(state, player);
    }

    virtual U evalTerminal(U value, const P& player, const int& depth) {
        hEvalUsed = true;
        return value;
    }

    virtual vector<A> orderActions(const S& state, const vector<A>& actions, const P& player, const int& depth) {
        return actions;
    }


public:

    // Constructor
    parIteDABSearch_tt(const VGame<S, A, P, U>& game, int startDepth, int maxTimeSeconds)
    : game(game), currentDepthLimit(startDepth), timer(maxTimeSeconds),  table(game.util_unknown)
    {}
    
    pair<A, U> makeDecision(S state) {
        metrics.reset();
        table.clear();    
        this->timer.start();
        auto player = game.getPlayer(state);
    
        auto actions = orderActions(state, game.getActions(state), player, 0);
        vector<actionUtility<A, U>> results;
        for (auto action : actions)
            results.push_back({action, game.util_min});
        
        do {
            incrementDepthLimit();
            hEvalUsed = false;
    
            vector<actionUtility<A, U>> newResults;
            
            int maxI = 0;
            //omp_set_num_threads(4);
            #pragma omp parallel for schedule(dynamic, 1)
            for (int i = 0; i < results.size(); i++) {
                auto actUtil = results[i];
                auto newState = game.getResult(state, actUtil.action);
                auto newUtil = minValue(newState, player, game.util_min, game.util_max, 1);
                
                // no break allowed, so we add the first results calculated up to the timeout
                // some threads might finish earlier than other with a smaller i, so I need to add
                // up to maxI results in the newResults (some might be with a smaller) with the previous
                // utility (if timeOut, the new utility is calculated at a smaller depth) 
                if (!timer.isTimeOut()) {
                    maxI = max(i, maxI);
                    actUtil.utility = newUtil;
                }
                
                if ( i <= maxI) {
                    #pragma omp critical
                    {newResults.push_back(actUtil);}
                }
            }

            // Sort the results
            std::sort(newResults.begin(), newResults.end());
    
            if (!newResults.empty()) {
                results = newResults;
                if (!timer.isTimeOut()) {
                    if (hasSafeWinner(results[0].utility))
                        break;
                    else if (results.size() > 1 && isSignificantlyBetter(results[0].utility, results[0].utility))
                        break;
                }
            }
            
        } while (!timer.isTimeOut() && hEvalUsed);
        
        return {results[0].action, results[0].utility};
    }

    std::string getMetrics() const {
        return metrics.toString();
    }
};

#endif // PARITEDEEPABSEARCHTT_H