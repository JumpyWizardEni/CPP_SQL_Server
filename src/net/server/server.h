#ifndef SQL_P3_SERVER_H
#define SQL_P3_SERVER_H
#include <string.h>
#include <regex>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "../../interpreter/dbInterpreter.h"
#include "../../interpreter/parser.h"
#define MAX_SIZE 1024

class Server {
    int socketDesc, clientSocket;
    std::string serverAnswer;
    socklen_t fromLen;
    sockaddr_in sin, fromSin;
public:
    void turnOn();
    void acceptConnection();
    bool receiveRequest();
    void turnOff() const;
    ~Server();
};


#endif //SQL_P3_SERVER_H
