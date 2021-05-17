#ifndef SQL_P3_TABLE_H
#define SQL_P3_TABLE_H

#include "entry.h"
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <unistd.h>



class SQLTable {
    std::string tableName;
    std::string fileName;
    int fileDesc;
    size_t currEntry; //current entry for reading
    Entry * entry;
    void checkEntryValidity(Entry *newEntry);
public:
    SQLTable(const std::string &newTableName, const std::string &, int newDesc, Entry *newEntry);

    void updateEntry(size_t position, Entry *newEntry);

    void addEntry(Entry *newEntry);

    void deleteEntry(size_t position);//delete (position + 1) entry

    void readEntry(Entry *readEntry, size_t position);//read (position + 1) entry

    int getNextEntry(Entry *readEntry);//for iterating

    Entry * readEntryInfo();
    ~SQLTable();
};

#endif //SQL_P3_TABLE_H
