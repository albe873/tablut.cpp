// Player1.cpp

#include <iostream>
#include <string>

#include <tablut/game.h>
#include <serverConnection/serverComunicator.h>

#include "tablut/custom_mtd.h"

using namespace std;

Move findBestMove(const Game& game, const State& state, int maxTime) {
    auto start = chrono::high_resolution_clock::now();

    cout << "Finding best move..." << endl;
    
    // search
    static custom_mtd<State, Move, Turn, int> search(game, 3, maxTime);
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

void notWelcomeMessage() {
    const char* quotes[] = {
        "Don't you have anything better to do?",
        "That's the trash can. Feel free to visit it anytime.",
        "Down here... It's kill or be killed!",
        "Do you wanna have a bad time?",
        "It's a beautiful day outside.\nBirds are singing, flowers are blooming...\nOn days like these, kids like you...\nShould be burning in hell.",
        "hey. what's up? this game's got pretty good graphics, huh? too bad the light's off so you can't see any of them."
    };
    
    srand((u_int)time(nullptr));
    int idx = rand() % (sizeof(quotes) / sizeof(quotes[0]));
    cout << quotes[idx] << endl;

}

void winMessage() {
    const char* quotes[] = {
        "This is all just a bad dream...\nAnd you're never waking up!\nHAHAHAHAHAHAHAHAHAHAHAHAHAHA",
        "(You feel... something)\n(You're filled with determination.)",
        "You Really Are An Idiot."
        "But nobody came"
    };

    int idx = rand() % (sizeof(quotes) / sizeof(quotes[0]));
    cout << quotes[idx] << endl;
}

void checkState(const State& serverState, const State& localState) {
    auto server_hash = serverState.hash();
    auto local_hash = localState.hash();
    if (server_hash != local_hash) {
        cerr << "Hash mismatch!" << endl;
        cerr << "Server hash: " << to_string(server_hash) << endl;
        cerr << "Local hash: " << to_string(local_hash) << endl;
        cerr << "Server state: " << endl << serverState.boardString() << endl;
        cerr << "Local state: " << endl << localState.boardString() << endl;
        cerr << "Server turn: " << to_string((int)serverState.getTurn()) << endl;
        cerr << "Local turn: " << to_string((int)localState.getTurn()) << endl;
        exit(1);
    }
}

int main(int argc, char* argv[]) {

    const string name = "Player1";

    int maxtime = 60;
    int safeTime = 4;

    // default team: white
    Turn team = Turn::White;

    // default server ip
    const char* ip = "localhost";
    // server port are in serverComunicator.h

    // used to check if the server state is the same as the local state
    // mainly for debugging and asserting game logic
    // DISABLED in the final version, i don't want to test edge cases that I never encontered
    // but surely will appen in the competition
    bool strictServerCheck = false;

    // get turn from first argument
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

    // get max time (in seconds) from second argument
    if (argc > 2) {
        maxtime = stoi(argv[2]);
        if (maxtime <= safeTime) {
            cerr << "Invalid time argument. Must be a positive integer and greater than safeTime." << endl;
            return 1;
        }
        maxtime -= safeTime;
    }

    // get server ip from third argument
    if (argc > 3) {
        ip = argv[3];
    }

    // get port from ServerComunicator
    // n.b. port is 5800 for white and 5801 for black
    int port = (team == Turn::White) ? ServerComunicator::whiteDefPort : ServerComunicator::blackDefPort;

    cout << "--------------------------------" << endl;
    cout << "Player1" << endl;
    cout << "--------------------------------" << endl;
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
    notWelcomeMessage();

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
        cout << "An error occurred" << endl;
    }

    if (state.getTurn() == Turn::Draw) {
        cout << u8"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣤⣄⢘⣒⣀⣀⣀⣀⠀⠀⠀" << endl;
        cout << u8"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣽⣿⣛⠛⢛⣿⣿⡿⠟⠂⠀" << endl;
        cout << u8"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣀⣀⡀⠀⣤⣾⣿⣿⣿⣿⣿⣿⣿⣷⣿⡆⠀" << endl;
        cout << u8"⠀⠀⠀⠀⠀⠀⣀⣤⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠁⠀" << endl;
        cout << u8"⠀⠀⠀⢀⣴⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀" << endl;
        cout << u8"⠀⠀⣠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀" << endl;
        cout << u8"⠀⠀⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠜⠀⠀⠀⠀⠀⠀⠀" << endl;
        cout << u8"⠀⠀⠀⢿⣿⣿⣿⣿⠿⠿⣿⣿⡿⢿⣿⣿⠈⣿⣿⣿⡏⣠⡴⠀⠀⠀⠀⠀⠀⠀" << endl;
        cout << u8"⠀⠀⣠⣿⣿⣿⡿⢁⣴⣶⣄⠀⠀⠉⠉⠉⠀⢻⣿⡿⢰⣿⡇⠀⠀⠀⠀⠀⠀⠀" << endl;
        cout << u8"⠀⠀⢿⣿⠟⠋⠀⠈⠛⣿⣿⠀⠀⠀⠀⠀⠀⠸⣿⡇⢸⣿⡇⠀⠀⠀⠀⠀⠀⠀" << endl;
        cout << u8"⠀⠀⢸⣿⠀⠀⠀⠀⠀⠘⠿⠆⠀⠀⠀⠀⠀⠀⣿⡇⠀⠿⠇⠀⠀⠀⠀⠀⠀⠀" << endl;
        cout << endl << "DRAW!" << endl;
    } else if ((team == Turn::White && state.getTurn() == Turn::WhiteWin) ||
               (team == Turn::Black && state.getTurn() == Turn::BlackWin)) {
        winMessage();
    }
    else if ((team == Turn::White && state.getTurn() == Turn::BlackWin) ||
             (team == Turn::Black && state.getTurn() == Turn::WhiteWin)) {
        cout << "GAME OVER" << endl;
        cout << "You cannot give up just yet..." << endl;
        cout << "Stay determined..." << endl;
    }


    return 0;
}
