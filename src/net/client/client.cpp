#include <cstring>
#include "client.h"
#include "../../exceptions.h"
#include <unistd.h>
#include <arpa/inet.h>

void Client::connectToServer(std::string host) {
    socketDesc = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(1234);
    inet_pton(AF_INET, host.c_str(), &sin.sin_addr);
    if (connect(socketDesc, (struct sockaddr *)&sin, sizeof(sin))) {
        throw NetException(strerror(errno));
    }
}

std::string Client::makeRequest(const std::string &buf) {
    sendto(socketDesc, buf.c_str(), buf.size() + 1, 0, (struct sockaddr *)&sin, sizeof(sin));
    char readed[MAX_SIZE];
    socklen_t size = sizeof(sin);
    if (recvfrom(socketDesc, readed, MAX_SIZE, 0, (struct sockaddr *)&sin, &size) == 0) {
        std::cout << "Server was turned off" << std::endl;
        disconnect();
        exit(0);
    };
    return std::string(readed);
}

void Client::disconnect() const {
    close(socketDesc);
}

