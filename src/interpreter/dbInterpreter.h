#ifndef SQL_P3_DBINTERPRETER_H
#define SQL_P3_DBINTERPRETER_H

#include <search.h>
#include "../db/bdFabric.h"
#include "regex"
#include "parser.h"
#include "../exceptions.h"
#include <algorithm>

class DBInterpreter {
    enum Mode {WHERE, UPDATE};
    Parser parser;
    std::string serverAnswer;
    void select();
    void insert();
    void update();
    void delete_();
    void create();
    void drop();
    FieldType stringToType(std::string &string);
    void addEntryToAnswer(Entry * entry, std::vector<size_t> &indices);
    int countLongExpression(Entry * entry, bool isUpdateFlag, Mode mode);
    std::string getFieldType(Entry *entry, const std::string & fieldName);
    bool countBoolExpression(Entry *entry, Mode mode);
    bool isContains(int number);
    bool isEntryAcceptable(Entry *entry, Mode mode);
    bool checkSample(const std::string &checkString, const std::string &sample);
public:
    std::string parseRequest(const char *request);
    DBInterpreter();

};


#endif //SQL_P3_DBINTERPRETER_H
