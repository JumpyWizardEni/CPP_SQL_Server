#include <iostream>
#include "user/user.h"
#include <csignal>
#include "exceptions.h"
User user;

void shutDown(int s) {
    exit(0);
}
int main(int argc, char const *argv[]) {
    signal(SIGINT, shutDown);
    signal(SIGTERM, shutDown);
    do {
        try {
            std::cout << user.makeRequest() << std::endl;
        } catch (const SQLException &e) {
            std::cout << e.getMessage() << std::endl;
        }
    } while (!user.isDisconnected());

    return 0;
}
