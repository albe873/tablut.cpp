// server.h

#include "simpleJson.h"
#include <netinet/in.h>

#ifndef SERVER_COMUNICATOR_H
#define SERVER_COMUNICATOR_H

class ServerComunicator {
private:
    int serverSocket;
    struct sockaddr_in serverAddress;
    char buffer[1024];

public :

    static const int whiteDefPort = 5800;
    static const int blackDefPort = 5801; 


    // Constructor
    ServerComunicator(const char* ip, int port);
    ~ServerComunicator();

    // Connection Methods
    bool connectToServer();
    void disconnectFromServer();
    bool isConnected() const;

    // Communication Methods
    bool sendName(std::string name);
    bool sendMove(Move m, Turn team);
    // new state without history
    State readState();

    // Utility Methods
    // add History to new state based on the old state 
    void addHistory(State& newState, const State& oldState);

};

#endif // SERVER_COMUNICATOR_H