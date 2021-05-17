#ifndef SQL_P3_BDFABRIC_H
#define SQL_P3_BDFABRIC_H

#include "table.h"
#include "repos.h"
#include <fcntl.h>
#include <unistd.h>

class Factory {
    static std::string generateFileName(const std::string &tableName);
public:

    static SQLTable *createTable(const std::string &newTableName, Entry * entry);

    static Entry *createEntry(std::vector<FieldType> &, std::vector<std::string> &);

    static void dropTable(const std::string &newTableName);

    static SQLTable *openTable(const std::string &openTableName);
};


#endif //SQL_P3_BDFABRIC_H
