#ifndef SQL_P3_CLIENT_H
#define SQL_P3_CLIENT_H
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include "../server/server.h"
#define MAX_SIZE 1024

class Client {
    int socketDesc;
    sockaddr_in sin;
public:
    void connectToServer(std::string host);
    void disconnect() const;
    std::string makeRequest(const std::string &buf);
};


#endif //SQL_P3_CLIENT_H
