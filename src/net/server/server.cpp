#include "server.h"
#include "../../exceptions.h"
#include "../../db/bdFabric.h"
#include <iostream>
#include <arpa/inet.h>

#define CHECK_SUCCESS(expr) if (expr < 0) {throw NetException(strerror(errno));}


void Server::turnOn() {
    CHECK_SUCCESS((socketDesc = socket(AF_INET, SOCK_STREAM, 0)));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(1234);
    sin.sin_addr.s_addr = INADDR_ANY;

    CHECK_SUCCESS(bind(socketDesc, (struct sockaddr *) &sin, sizeof(sin)));
    CHECK_SUCCESS(listen(socketDesc, 1));
}


void Server::turnOff() const {
    close(socketDesc);
}

void Server::acceptConnection() {
    fromLen = sizeof(fromSin);
    CHECK_SUCCESS((clientSocket = accept(socketDesc, (struct sockaddr *) &fromSin, &fromLen)))
    std::cout << "Client connected, sockaddr: " << fromSin.sin_addr.s_addr << std::endl;

}

bool Server::receiveRequest() {
    char buffer[MAX_SIZE];
    fromLen = sizeof(fromSin);
    int readed = recvfrom(clientSocket, buffer, MAX_SIZE, 0, (struct sockaddr *) &fromSin, &fromLen);
    if (readed <= 0) {
        std::cout << "Client disconnected" << std::endl;
        return false;
    }
    std::cout << "Request: " << buffer << std::endl;
    DBInterpreter interpreter;
    try {
        serverAnswer = interpreter.parseRequest(buffer);
    } catch (const SQLException & e) {
        serverAnswer = e.getMessage();
    }
    sendto(clientSocket, serverAnswer.c_str(), serverAnswer.size() + 1, 0, (struct sockaddr *) &fromSin, fromLen);
    return true;
}

Server::~Server() {
    this->turnOff();
}

