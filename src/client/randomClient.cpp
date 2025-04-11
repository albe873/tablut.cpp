#include <iostream>
#include <string>

#include <tablut/common.h>
#include <tablut/state.h>
#include <tablut/result.h>
#include <serverConnection/serverComunicator.h>


int main(int argc, char* argv[]) {

    std::string name = "randomPlayer";

    int maxtime = 60;
    Turn team = Turn::White;
    const char* ip = "localhost";

    if (argc > 1) {
        std::string arg1 = argv[1];
        std::transform(arg1.begin(), arg1.end(), arg1.begin(), ::toupper);
        if (arg1 == "WHITE") {
            team = Turn::White;
        } else if (arg1 == "BLACK") {
            team = Turn::Black;
        }
        else {
            std::cerr << "Invalid team argument. Use 'white' or 'black' (case insensitive)." << std::endl;
            return 1;
        }
    }
    if (argc > 2) {
        maxtime = std::stoi(argv[2]);
        if (maxtime <= 0) {
            std::cerr << "Invalid time argument. Must be a positive integer." << std::endl;
            return 1;
        }
    }
    if (argc > 3) {
        ip = argv[3];
    }

    int port = (team == Turn::White) ? ServerComunicator::whiteDefPort : ServerComunicator::blackDefPort;

    std::cout << "Connecting to server at " << ip << " with max time " << std::to_string(maxtime) << " seconds." << std::endl;
    std::cout << "Team: " << (team == Turn::White ? "White" : "Black") << std::endl;


    ServerComunicator client(ip, port);

    // connection
    if (!client.connectToServer()) {
        std::cerr << "Failed to connect to server." << std::endl;
        return 1;
    }
    std::cout << "Connected to server." << std::endl;

    // send name
    if (!client.sendName(name)) {
        std::cerr << "Failed to send name to server." << std::endl;
        client.disconnectFromServer();
        return 1;
    }
    std::cout << "Name sent to server." << std::endl;

    State state;
    Turn turn;
    while (true) {
        // read state
        state = client.readState();
        turn = state.getTurn();
        if (turn == Turn::BlackWin || turn == Turn::WhiteWin || turn == Turn::Draw || turn == (Turn) -1) {
            break;
        }
        if (turn != team) {
            //std::cout << "Not your turn." << std::endl;
            continue;
        }
        std::cout << "State read: \n" << state.boardString() << std::endl;

        // get possible moves
        std::vector<Move> moves = Action::getActions(state);
        if (moves.empty()) {
            std::cerr << "No possible moves." << std::endl;
            return 1;
        }

        // select a random move
        Move move = moves[rand() % moves.size()];

        // Debug the move coordinates before sending
        std::cout << "Selected move from: [" << std::to_string(move.getFrom().x) << "," << std::to_string(move.getFrom().y) 
                  << "] to: [" << std::to_string(move.getTo().x) << "," << std::to_string(move.getTo().y) << "]" << std::endl;
    
        // send move to server
        if (!client.sendMove(move, team)) {
            std::cerr << "Failed to send move to server." << std::endl;
            break;
        }

        State result = Result::applyAction(state, move);
        std::cout << "Expected result:" << std::endl;
        std::cout << result.boardString() << std::endl;
    }
    client.disconnectFromServer();

    if (state.getTurn() == Turn::WhiteWin) {
        std::cout << "White wins!" << std::endl;
    } else if (state.getTurn() == Turn::BlackWin) {
        std::cout << "Black wins!" << std::endl;
    } else if (state.getTurn() == Turn::Draw) {
        std::cout << "Game draw!" << std::endl;
    } else {
        std::cout << "WTF happened?" << std::endl;
    }

    return 0;
}