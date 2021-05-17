#ifndef SQL_P3_USER_H
#define SQL_P3_USER_H
#include <string>
#include "../net/client/client.h"

class User {
    Client *client;
    void prepareClient();
public:
    User();
    void disconnect();
    bool isDisconnected();
    std::string makeRequest();
    ~User();
};


#endif //SQL_P3_USER_H
