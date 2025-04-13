#include <iostream>
#include <string>

#include <tablut/common.h>
#include <tablut/state.h>
#include <tablut/result.h>
#include <tablut/game.h>
#include <tablut/heuristics.h>
#include <serverConnection/serverComunicator.h>
#include <adversarialSearch/iteDeepAlphaBetaSearch.h>
#include <adversarialSearch/parIteDABSearch.h>

using namespace std;

Move findBestMove(const Game& game, const State& state, int maxTime) {
    cout << "Finding best move..." << endl;
    //auto search = IteDeepAlphaBetaSearch<State, Move, Turn, int8_t>(game, Heuristics::min, Heuristics::max, maxTime);
    auto search = parIteDABSearch<State, Move, Turn, int8_t>(game, Heuristics::min, Heuristics::max, maxTime);
    Move bestMove = search.makeDecision(state);
    cout << "Metrics: " << search.getMetrics() << endl;

    auto bestValue = Heuristics::getHeuristics(state, state.getTurn());
    cout << "Best move found, value: " << to_string(bestValue) << endl;
    return bestMove;
}

void checkState(const State& serverState, const State& localState) {
    int8_t expectedHash = localState.softHash();
    int8_t actualHash = serverState.hash();
    if (expectedHash != actualHash) {
        cerr << "State hash mismatch. Server: " << to_string(expectedHash) 
             << ", Local: " << to_string(actualHash) << endl;
        cerr << "Server state: " << endl << serverState.boardString() << endl;
        cerr << "Local state: " << endl << localState.boardString() << endl;
        exit(1);
    }
}

int main(int argc, char* argv[]) {

    string name = "aiPlayer.cpp";

    int maxtime = 5;
    Turn team = Turn::White;
    const char* ip = "localhost";
    bool strictServerCheck = true;

    if (argc > 1) {
        string arg1 = argv[1];
        transform(arg1.begin(), arg1.end(), arg1.begin(), ::toupper);
        if (arg1 == "WHITE") {
            team = Turn::White;
        } else if (arg1 == "BLACK") {
            team = Turn::Black;
        }
        else {
            cerr << "Invalid team argument. Use 'white' or 'black' (case insensitive)." << endl;
            return 1;
        }
    }
    if (argc > 2) {
        maxtime = stoi(argv[2]);
        if (maxtime <= 0) {
            cerr << "Invalid time argument. Must be a positive integer." << endl;
            return 1;
        }
    }
    if (argc > 3) {
        ip = argv[3];
    }
    if (argc > 4) {
        string arg4 = argv[4];
        transform(arg4.begin(), arg4.end(), arg4.begin(), ::toupper);
        if (arg4 == "CS") {
            strictServerCheck = true;
        } else {
            cerr << "Invalid strict server check argument. Use 'true' or 'false' (case insensitive)." << endl;
            return 1;
        }
    }

    int port = (team == Turn::White) ? ServerComunicator::whiteDefPort : ServerComunicator::blackDefPort;

    cout << "Connecting to server at " << ip << " with max time " << to_string(maxtime) << " seconds." << endl;
    cout << "Team: " << (team == Turn::White ? "White" : "Black") << endl;


    ServerComunicator client(ip, port);

    // connection
    if (!client.connectToServer()) {
        cerr << "Failed to connect to server." << endl;
        return 1;
    }
    cout << "Connected to server." << endl;

    // send name
    if (!client.sendName(name)) {
        cerr << "Failed to send name to server." << endl;
        client.disconnectFromServer();
        return 1;
    }
    cout << "Name sent to server." << endl;

    Game game = Game(State(), Action::getActions, Result::applyAction, Heuristics::getHeuristics);

    State state, result;
    Turn turn;
    while (true) {
        // read state
        state = client.readState();
        turn = state.getTurn();
        if (turn == Turn::BlackWin || turn == Turn::WhiteWin || turn == Turn::Draw || turn == (Turn) -1) {
            break;
        }
        if (turn != team) {
            cout << "Not your turn." << endl;
            if (strictServerCheck)
                checkState(state, result);
            continue;
        }
        cout << "State read: \n" << state.boardString() << endl;

        // get possible moves
        vector<Move> moves = Action::getActions(state);
        if (moves.empty()) {
            cerr << "No possible moves." << endl;
            return 1;
        }

        // select a random move
        //Move move = moves[rand() % moves.size()];
        Move move = findBestMove(game, state, maxtime);

        // Debug the move coordinates before sending
        cout << "Selected move from: [" << to_string(move.getFrom().x) << "," << to_string(move.getFrom().y) 
             << "] to: [" << to_string(move.getTo().x) << "," << to_string(move.getTo().y) << "]" << endl;
    
        // send move to server
        if (!client.sendMove(move, team)) {
            cerr << "Failed to send move to server." << endl;
            break;
        }

        result = Result::applyAction(state, move);
    }
    client.disconnectFromServer();

    cout << "\n\nGAME OVER, FINAL STATE: \n\n" << state.boardString() << endl;

    if (state.getTurn() == Turn::WhiteWin) {
        cout << "White wins!" << endl;
    } else if (state.getTurn() == Turn::BlackWin) {
        cout << "Black wins!" << endl;
    } else if (state.getTurn() == Turn::Draw) {
        cout << "Game draw!" << endl;
    } else {
        cout << "WTF happened?" << endl;
    }

    return 0;
}