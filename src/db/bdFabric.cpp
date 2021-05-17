#include <cstring>
#include "bdFabric.h"
#include "fileInOut.h"
#include "../exceptions.h"

SQLTable *Factory::createTable(const std::string &newTableName, Entry * entry) {
    std::string fileName = generateFileName(newTableName);
    int fDesc;
    if ((fDesc = open(fileName.c_str(), O_CREAT | O_RDWR | O_EXCL, S_IRWXU | S_IRWXG | S_IRWXO)) < 0){
        throw TableCreatingException(strerror(errno));
    }
    FileInOut::writeEntryInfo(fDesc, entry);
    auto newTable = new SQLTable(newTableName, fileName, fDesc, entry);
    SQLRepository::getRepository().setTable(newTableName, newTable);
    return newTable;
}

void Factory::dropTable(const std::string &newTableName) {
    delete SQLRepository::getRepository().getTable(newTableName);
    if (unlink(generateFileName(newTableName).c_str()) < 0) {
        throw TableCreatingException(strerror(errno));
    }
    SQLRepository::getRepository().deleteTable(newTableName);
}

SQLTable *Factory::openTable(const std::string &openTableName) {
    std::string fileName = generateFileName(openTableName);
    int fDesc;
    if ((fDesc= open(fileName.c_str(), O_EXCL | O_RDWR, "rw")) < 0){
        throw TableCreatingException(strerror(errno));
    }
    SQLTable * openTable =(SQLTable *) new SQLTable(openTableName, fileName, fDesc, FileInOut::readEntryInfo(fDesc));
    SQLRepository::getRepository().setTable(openTableName, openTable);
    return openTable;
}

Entry *Factory::createEntry(std::vector<FieldType> &fieldsTypes, std::vector<std::string> &fieldsNames) {
    Entry *entry =(Entry *) new Entry();
    for (size_t i = 0; i < fieldsTypes.size(); ++i) {
       entry->setField(fieldsTypes[i], fieldsNames[i]);
    }
    SQLRepository::getRepository().setEntry(entry);
    return entry;
}

std::string Factory::generateFileName(const std::string &tableName) {
    return tableName + ".txt";
}

