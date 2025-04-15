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

    state.movePiece({4, 4}, {6, 6});
    state.movePiece({8, 3}, {5, 3});
    state.movePiece({1, 4}, {1, 7});
    state.removePiece({4, 7});
    state.movePiece({4, 5}, {3, 6});
    state.movePiece({4, 6}, {6, 7});
    state.movePiece({7, 4}, {6, 4});
    state.removePiece({5, 4});
SOLVED
*/

/*
State read: 
B - - + B + - - - 
- - - - + - - - - 
- - - W - B B - - 
B - - W - W - - B 
B B W - + - K + B 
B - - W W - - - B 
- - - - W W - - - 
- - B - + - - - - 
- - - B B B - - - 

Finding best move...
Metrics: Max Depth: 5, Nodes Expanded: 22398587
Best move found, value: 32
Time taken to find best move: 8000 ms
Selected move from: [0,3] to: [0,1]
Sending move: {"from":"a4","to":"a2","turn":"BLACK"}
Not your turn.


GAME OVER, FINAL STATE: 

B - - + B + - - - 
B - - - + - - - - 
- - - W - B B - - 
+ - - W - W - - B 
B B W - + - - + B 
B - - W W - - - B 
- - - - W W - - - 
- - B - + - - - - 
- - - B B B K - - 

White wins!


*/


int main (int argc, char **argv) {
    Piece pieces[9][9] = {
        {Piece::Black, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Black, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty},
        {Piece::Black, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty},
        {Piece::Empty, Piece::Empty, Piece::Empty, Piece::White, Piece::Empty, Piece::Black, Piece::Black, Piece::Empty, Piece::Empty},
        {Piece::Empty, Piece::Empty, Piece::Empty, Piece::White, Piece::Empty, Piece::White, Piece::Empty, Piece::Empty, Piece::Black},
        {Piece::Black, Piece::Black, Piece::White, Piece::Empty, Piece::Empty, Piece::Empty, Piece::King,  Piece::Empty, Piece::Black},
        {Piece::Black, Piece::Empty, Piece::Empty, Piece::White, Piece::White, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Black},
        {Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::White, Piece::White, Piece::Empty, Piece::Empty, Piece::Empty},
        {Piece::Empty, Piece::Empty, Piece::Black, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty},
        {Piece::Empty, Piece::Empty, Piece::Empty, Piece::Black, Piece::Black, Piece::Black, Piece::Empty, Piece::Empty, Piece::Empty}
    };
    
    auto state = State(pieces, Turn::Black);
    std::cout << state.boardString() << std::endl;

    int maxTime = atoi(argv[1]);

    auto start = chrono::high_resolution_clock::now();
    Game game = Game(State(), Action::getActions, Result::applyAction, Heuristics::getHeuristics);

    cout << "Finding best move..." << endl;
    
    
    // search
    auto search = parIteDABSearch<State, Move, Turn, int8_t>(game, Heuristics::min, Heuristics::max, 3, maxTime);
    //auto search = IteDeepAlphaBetaSearch<State, Move, Turn, int8_t>(game, Heuristics::min, Heuristics::max, maxTime);
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
    std::cout << std::to_string( (int) state.getTurn()) << std::endl;

    // white
    auto search2 = parIteDABSearch<State, Move, Turn, int8_t>(game, Heuristics::min, Heuristics::max, 3, maxTime);
    //auto search2 = IteDeepAlphaBetaSearch<State, Move, Turn, int8_t>(game, Heuristics::min, Heuristics::max, maxTime);
    move = search2.makeDecision(state);
    cout << "Metrics: " << search2.getMetrics() << endl;
    bestValue = Heuristics::getHeuristics(state, state.getTurn());
    cout << "Best move found, value: " << to_string(bestValue) << endl;
    cout << "Selected move from: [" << to_string(move.getFrom().x) << "," << to_string(move.getFrom().y)
    << "] to: [" << to_string(move.getTo().x) << "," << to_string(move.getTo().y) << "]" << endl;
    state = Result::applyAction(state, move);
    std::cout << state.boardString() << std::endl;
    std::cout << std::to_string( (int) state.getTurn()) << std::endl;
}