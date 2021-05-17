#include <iostream>
#include "repos.h"
#include "../exceptions.h"

SQLRepository &SQLRepository::getRepository() {
    static SQLRepository instance;
    return instance;
}

SQLTable *SQLRepository::getTable(const std::string &getTableName) {
    if (tables.find(getTableName) != tables.end()) {
        return tables[getTableName];
    }
    return nullptr;
}

void SQLRepository::setTable(const std::string &setTableName, SQLTable *table) {
    if (tables.find(setTableName) == tables.end()) {
        tables[setTableName] = table;
    }
}

SQLRepository::~SQLRepository() {
    for (auto & table : tables) {
        delete table.second;
    }
    for (auto & entry : entries) {
        delete entry;
    }
}

void SQLRepository::deleteTable(const std::string &deleteTableName) {
    if (tables.find(deleteTableName) != tables.end()) {
        tables.erase(deleteTableName);
    }
}

void SQLRepository::setEntry(Entry *newEntry) {
    entries.push_back(newEntry);
}

