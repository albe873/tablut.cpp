// aiPlayer2.cpp

#include <iostream>
#include <string>

#include <tablut/game.h>
#include <serverConnection/serverComunicator.h>

#include "customSearch.cpp"

using namespace std;

Move findBestMove(const Game& game, const State& state, int maxTime) {
    auto start = chrono::high_resolution_clock::now();

    cout << "Finding best move..." << endl;
    
    // search
    static CustomSearch<State, Move, Turn, int> search(game, 4, maxTime);
    //search.setMaxTime(maxTime);
    auto bestAction = search.makeDecision(state);
    
    // metrics
    cout << "Metrics: " << search.getMetrics() << endl;
    auto bestValue = Heuristics::getHeuristics(state, state.getTurn());
    cout << "Best move found, value: " << to_string(bestValue) << " playing for: " << to_string(bestAction.second) << endl;

    // time taken
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Time taken to find best move: " << duration.count() << " ms" << endl;
    
    return bestAction.first;
}

void checkState(const State& serverState, const State& localState) {
    auto server_hash = serverState.hash64();
    auto local_hash = localState.hash64();
    if (server_hash != local_hash) {
        cerr << "Hash mismatch!" << endl;
        cerr << "Server hash: " << to_string(server_hash) << endl;
        cerr << "Local hash: " << to_string(local_hash) << endl;
        cerr << "Server state: " << endl << serverState.boardString() << endl;
        cerr << "Local state: " << endl << localState.boardString() << endl;
        exit(1);
    }
}

int main(int argc, char* argv[]) {

    const string name = "Player1";

    int maxtime = 60;
    int safeTime = 2;
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
        if (maxtime <= safeTime) {
            cerr << "Invalid time argument. Must be a positive integer and greater than safeTime." << endl;
            return 1;
        }
        maxtime -= safeTime;
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

    Game game = Game(State(), 
                     Action::getActions, 
                     Result::applyAction, 
                     Heuristics::getHeuristics, 
                     Heuristics::min, Heuristics::max, Heuristics::unknown);

    bool first_move = true;
    State state, oldState, result;
    Turn turn;
    while (true) {
        // read state
        state = client.readState();
        turn = state.getTurn();

        // game end check
        if (turn == Turn::BlackWin || turn == Turn::WhiteWin || turn == Turn::Draw || turn == (Turn) -1)
            break;

        // check if is my turn
        if (turn != team) {
            cout << "Not your turn." << endl;
            if (strictServerCheck)
                checkState(state, result);
            
            // continue, wait and read another state
            continue;
        }

        // my turn
        cout << "State read: \n" << state.boardString() << endl;

        // the state received has no history
        // add history to the new received state (for Draw)
        // special case: the first move has no history -> no copy
        if (!first_move)
            client.addHistory(state, oldState);
        else
            first_move = false;
        
        oldState = state;

        // get possible moves
        vector<Move> moves = Action::getActions(state);
        if (moves.empty()) {
            cerr << "No possible moves." << endl;
            return 1;
        }

        // find the best move
        Move move = findBestMove(game, state, maxtime);

        // Print the selected move
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