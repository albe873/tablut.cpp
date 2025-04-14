#include <iostream>
#include <string>
#include <chrono>
#include <tablut/common.h>
#include <tablut/state.h>
#include <tablut/game.h>
#include <tablut/heuristics.h>
#include <tablut/actions.h>
#include <tablut/result.h>
#include <adversarialSearch/iteDeepAlphaBetaSearch.h>
#include <adversarialSearch/parIteDABSearch.h>

using namespace std;
/*

State read: 
- - - B B B - - - 
- - - - B - - - - 
- - - - W - - - - 
B - - - W B - - + 
B + W W + - B + B 
B - - - - - - - B 
- - - W - - K - - 
- B - - + - W - - 
- - - B B B - - - 

*/


int main (int argc, char **argv) {
    State state;
    state.movePiece({4, 4}, {6, 6});
    state.movePiece({8, 3}, {5, 3});
    state.movePiece({1, 4}, {1, 7});
    state.removePiece({4, 7});
    state.movePiece({4, 5}, {3, 6});
    state.movePiece({4, 6}, {6, 7});
    state.movePiece({7, 4}, {6, 4});
    state.removePiece({5, 4});
    std::cout << state.boardString() << std::endl;
    state.setTurn(Turn::Black);

    int maxTime = atoi(argv[1]);

    auto start = chrono::high_resolution_clock::now();
    Game game = Game(State(), Action::getActions, Result::applyAction, Heuristics::getHeuristics);

    cout << "Finding best move..." << endl;
    
    
    // search
    auto search = parIteDABSearch<State, Move, Turn, int8_t>(game, Heuristics::min, Heuristics::max, 4, maxTime);
    Move move = search.makeDecision(state);
    
    // metrics
    cout << "Metrics: " << search.getMetrics() << endl;
    auto bestValue = Heuristics::getHeuristics(state, state.getTurn());
    cout << "Best move found, value: " << to_string(bestValue) << endl;

    // Print the selected move
    cout << "Selected move from: [" << to_string(move.getFrom().x) << "," << to_string(move.getFrom().y) 
    << "] to: [" << to_string(move.getTo().x) << "," << to_string(move.getTo().y) << "]" << endl;

    // time taken
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout << "Time taken to find best move: " << duration.count() << " ms" << endl;

    state = Result::applyAction(state, move);
    std::cout << state.boardString() << std::endl;
}