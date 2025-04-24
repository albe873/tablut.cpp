// new_test.cpp
// various tests for debug

#include <iostream>
#include <string>
#include <chrono>

#include <tablut/game.h>
#include <tablut/customSearch.h>

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
SOLVED


State read: 
B - - + B B - - - 
- - - - + - B - - 
- - W - - - - - - 
+ - - - W W - - B 
B B W W + B - + + 
B - - - - - - - B 
- W - - - W K B - 
- - B - + - W - - 
- B - + + + - - - 

Finding best move...
Metrics: Max Depth: 6, Nodes Expanded: 2587037
Best move found, value: 27 playing for: -100
Time taken to find best move: 7782 ms
Selected move from: [0,5] to: [7,5]
Sending move: {"from":"a6","to":"h6","turn":"BLACK"}
Not your turn.
State read: 
B - - + B B - - - 
- - - - + - B - - 
- - W - - - - - - 
+ - - - W W - - B 
B B W W + B - + + 
+ - - - - - - B B 
- W - - - W K B - 
- - B - + W - - - 
- B - + + + - - - 

Finding best move...
Metrics: Max Depth: 2, Nodes Expanded: 128
Best move found, value: 24 playing for: -100
Time taken to find best move: 0 ms
Selected move from: [8,3] to: [8,1]
Sending move: {"from":"i4","to":"i2","turn":"BLACK"}
Not your turn.


GAME OVER, FINAL STATE: 

B - - + B B - - - 
- - - - + - B - B 
- - W - - - - - - 
+ - - - W W - - + 
B B W W + B - + + 
+ - - - - - - B B 
- W - - - W - B - 
- - B - + W - - - 
- B - + + + K - - 

White wins!












Sending move: {"from":"g2","to":"g4","turn":"WHITE"}
Not your turn.
State read: 
- - - B B + - - - 
- - B - + - - - - 
- B - - W - - B - 
B - W - - B W K B 
B + W W + - - + B 
B - - - W - - - + 
- - - - W - - - - 
- - - - B - - - - 
- - - B B B - - - 


State read: 
- - - B B B - - - 
- - - - B - - - - 
- - - - W - W B B 
B - - - W - - - + 
B B W W + - - + + 
B - - - W - - - + 
- - - - W - - K B 
- - - - + - - B - 
- - - B B B - - - 

Finding best move...
Metrics: Max Depth: 4, Nodes Expanded: 1277911, TT Miss: 1210468, TT Hit: 62030
Best move found, value: -215 playing for: 1000
Time taken to find best move: 15233 ms
Selected move from: [6,2] to: [6,7]
Sending move: {"from":"g3","to":"g8","turn":"WHITE"}
Not your turn.


*/


int main (int argc, char **argv) {
    Piece pieces[9][9] = {
        {Piece::Empty, Piece::Empty, Piece::Empty, Piece::Black, Piece::Black, Piece::Black, Piece::Empty, Piece::Empty, Piece::Empty},
        {Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Black, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty},
        {Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::White, Piece::Empty, Piece::White, Piece::Black, Piece::Black},
        {Piece::Black, Piece::Empty, Piece::Empty, Piece::Empty, Piece::White, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty},
        {Piece::Black, Piece::Black, Piece::White, Piece::White, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty},
        {Piece::Black, Piece::Empty, Piece::Empty, Piece::Empty, Piece::White, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Black},
        {Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::White, Piece::Empty, Piece::Empty, Piece::King, Piece::Black},
        {Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Black, Piece::Empty},
        {Piece::Empty, Piece::Empty, Piece::Empty, Piece::Black, Piece::Black, Piece::Black, Piece::Empty, Piece::Empty, Piece::Empty}
    };
    
    auto state = State(pieces, Turn::White);
    std::cout << state.boardString() << std::endl;

    int maxTime = atoi(argv[1]);

    auto start = chrono::high_resolution_clock::now();
    Game game = Game(State(), Action::getActions, Result::applyAction, Heuristics::getHeuristics, 
                    Heuristics::min, Heuristics::max, Heuristics::unknown);

    cout << "Finding best move..." << endl;
    
    
    // search
    auto search = CustomSearch<State, Move, Turn, int>(game, 3, maxTime);
    auto result = search.makeDecision(state);
    Move move = result.first;
    
    // metrics
    cout << "Metrics: " << search.getMetrics() << endl;
    auto bestValue = Heuristics::getHeuristics(state, state.getTurn());
    cout << "Best move found, value: " << to_string(bestValue) << " Playing for "<< result.second << endl;

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




    // second move
    auto search2 = CustomSearch<State, Move, Turn, int>(game, 3, maxTime);
    //auto search2 = IteDeepAlphaBetaSearch<State, Move, Turn, int>(game, Heuristics::min, Heuristics::max, maxTime);
    move = search2.makeDecision(state).first;
    cout << "Metrics: " << search2.getMetrics() << endl;
    bestValue = Heuristics::getHeuristics(state, state.getTurn());
    cout << "Best move found, value: " << to_string(bestValue) << endl;
    cout << "Selected move from: [" << to_string(move.getFrom().x) << "," << to_string(move.getFrom().y)
    << "] to: [" << to_string(move.getTo().x) << "," << to_string(move.getTo().y) << "]" << endl;
    state = Result::applyAction(state, move);
    std::cout << state.boardString() << std::endl;
    std::cout << std::to_string( (int) state.getTurn()) << std::endl;
}