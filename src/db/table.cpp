#include "table.h"
#include "fileInOut.h"
#include "../exceptions.h"


SQLTable::SQLTable(const std::string &newTableName, const std::string &newFileName, int newDesc, Entry *newEntry) {
    tableName = newTableName;
    fileName = newFileName;
    fileDesc = newDesc;
    currEntry = 0;
    entry = newEntry;
}

SQLTable::~SQLTable() {
}

void SQLTable::addEntry(Entry *newEntry) {
    checkEntryValidity(newEntry);
    lseek(fileDesc, 0, SEEK_END);
    FileInOut::writeEntry(fileDesc, newEntry);
}

void SQLTable::deleteEntry(size_t position) {
    FileInOut::deleteEntry(fileDesc, fileName, position);
    if (position <= currEntry) {
        currEntry--;
    }
}

void SQLTable::updateEntry(size_t position, Entry *newEntry) {
    checkEntryValidity(newEntry);
    FileInOut::deleteEntry(fileDesc, fileName, position);
    if (position <= currEntry) {
        currEntry--;
    }
    FileInOut::writeEntry(fileDesc, newEntry);

}

Entry * SQLTable::readEntryInfo() {
    return FileInOut::readEntryInfo(fileDesc);
}

void SQLTable::readEntry(Entry *readEntry, size_t position) {
    for (size_t i = 0; i < entry->fields.size(); ++i) {
        readEntry->setField(entry->fields[i]->getTypeEnum(), entry->fields[i]->getFieldName());
    }
    FileInOut::readEntry(fileDesc, readEntry, position);
}

int SQLTable::getNextEntry(Entry *readEntry) {
    if (FileInOut::seekToEntry(fileDesc, currEntry) == -1) {
        return -1;
    }
    FileInOut::readEntry(fileDesc, readEntry, currEntry);
    currEntry++;
    return (int)currEntry - 1;
}

void SQLTable::checkEntryValidity(Entry *newEntry) {
    if (newEntry->fields.size() != entry->fields.size()) {
        throw TableEditingException("SQLTable::addEntry: new entry must repeat table structure");
    }
    for (size_t i = 0; i < entry->fields.size(); ++i) {
        if (newEntry->fields[i]->getType() != entry->fields[i]->getType()) {
            throw TableEditingException("SQLTable::addEntry: new entry must repeat table structure");
        }
    }
}

