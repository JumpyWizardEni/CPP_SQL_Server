#include "user.h"
std::string User::makeRequest() {
    if (client == nullptr) {
        prepareClient();
    }
    std::string request = "";
    int currentChar;
    while ((currentChar = getchar()) != '\n') {
        request += currentChar;
    }
    return client->makeRequest(request);
}

void User::prepareClient() {
    std::string ip = "127.0.0.1";
    client = new Client();
    client->connectToServer(ip);
}

User::~User() {
    disconnect();
}



User::User() {
    client = nullptr;
}

void User::disconnect() {
    if(client != nullptr) {
        client->disconnect();
        delete client;
    }
}

bool User::isDisconnected() {
    return client == NULL;
}
