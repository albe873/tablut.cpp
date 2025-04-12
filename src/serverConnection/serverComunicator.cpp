// serverComunicator.cpp

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#include "serverConnection/serverComunicator.h"

// ------ Constructor ------

ServerComunicator::ServerComunicator(const char* ip, int port) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serverAddress.sin_addr);
}

ServerComunicator::~ServerComunicator() {
    disconnectFromServer();
}


// ------ Connection Methods ------

bool ServerComunicator::connectToServer() {
    if (connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        return false;
    }
    return true;
}

void ServerComunicator::disconnectFromServer() {
    if (serverSocket > 0) {
        close(serverSocket);
        serverSocket = -1;
    }
}

bool ServerComunicator::isConnected() const {
    return (serverSocket > 0);
}


// ------ Communication Methods ------

bool ServerComunicator::sendName(std::string name) {
    if (!isConnected()) return false;
    
    //name = SimpleJson::toJson(name);
    int len = name.size();

    // Convert length to network byte order (DataInputStream.readInt() expects a 4-byte big-endian integer)
    int netLen = htonl(len);

    int bytesSent = send(serverSocket, &netLen, sizeof(netLen), 0);
    bytesSent += send(serverSocket, name.c_str(), len, 0);
    return (bytesSent == (int)len + (int)sizeof(netLen));
}

bool ServerComunicator::sendMove(Move m, Turn team) {
    std::string moveData = SimpleJson::toJson(m, team);
    std::cout << "Sending move: " << moveData << std::endl;
    if (!isConnected())
        return false;
    int len = moveData.size();
    int netLen = htonl(len);
    int bytesSent = send(serverSocket, &netLen, sizeof(netLen), 0);
    bytesSent += send(serverSocket, moveData.c_str(), moveData.size(), 0);
    return (bytesSent == (int)moveData.size() + (int)sizeof(netLen));
}

State ServerComunicator::readState() {
    State s;
    s.setTurn((Turn) -1);
    if (!isConnected()) {
        std::cerr << "Not connected to server." << std::endl;
        return s;
    }
    int netLen;
    int bytesRead = recv(serverSocket, &netLen, sizeof(netLen), MSG_WAITALL);
    if (bytesRead <= 0) {
        //std::cerr << "Failed to read state length." << std::endl;
        return s;
    }
    netLen = ntohl(netLen);

    std::string jsonData(netLen, '\0');
    bytesRead = recv(serverSocket, &jsonData[0], netLen, MSG_WAITALL);
    if (bytesRead <= 0) {
        std::cerr << "Failed to read state data." << std::endl;
        return s;
    }
    s = SimpleJson::fromJson(jsonData);

    return s; 
}

// ------ Utility Methods ------

void ServerComunicator::addHistoty(State& newState, const State& oldState) {
    auto history = oldState.getHistory();
    auto softHash = newState.softHash();
    history.push_back(softHash);
    newState.setHistory(history);
}