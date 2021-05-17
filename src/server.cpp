#include "exceptions.h"
#include <iostream>
#include <csignal>
#include "net/server/server.h"
Server server;

void shutDown(int s) {
    server.turnOff();
    exit(0);
}
int main(int argc, char const *argv[])
{

    signal(SIGINT, shutDown);
    try {
        server.turnOn();
        while(true) {
            server.acceptConnection();

            while(server.receiveRequest()) {
            }
        }
    } catch(SQLException &e) {
        std::cout << e.getMessage() << std::endl;
    }

    return 0;
}
