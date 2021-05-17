#ifndef SQL_P3_SQLTABLE_H
#define SQL_P3_SQLTABLE_H

#include "table.h"
#include <map>

class SQLRepository {
    std::map<std::string, SQLTable *> tables;
    std::vector<Entry *> entries;

    SQLRepository() {
    };

    SQLRepository(const SQLRepository &oldRepos) {
    };

    SQLRepository &operator=(const SQLRepository &oldRepos) {return *this;};

public:
    static SQLRepository &getRepository();

    ~SQLRepository();

    SQLTable *getTable(const std::string &getTableName);

    void setTable(const std::string &setTableName, SQLTable *table);

    void deleteTable(const std::string &deleteTableName);

    void setEntry(Entry * newEntry);

};


#endif //SQL_P3_SQLTABLE_H
