
#include "fileInOut.h"
#include "bdFabric.h"
#include "../exceptions.h"
#include <iostream>

Entry *FileInOut::readEntryInfo(int fileDesc) {
    lseek(fileDesc, 0, SEEK_SET);
    char buffer[BUFFER_SIZE];
    std::string currentWord = "";
    char currChar;
    size_t readedSize;
    int currentLine = 0;
    std::vector<FieldType> fieldTypes;
    std::vector<std::string> fieldsNames;
    if ((readedSize = read(fileDesc, buffer, BUFFER_SIZE)) == 0) {
        throw IOException("FileInOut::readEntryInfo: file is empty.");
    };
    while (readedSize) {
        for (size_t i = 0; i < readedSize; i++) {
            currChar = buffer[i];
            if (currChar != ' ') {
                if (currChar != '\n') {
                    currentWord += currChar;
                }
            } else {
                if (currentLine == 0) {
                    if (currentWord == "Long") {
                        fieldTypes.push_back(FieldType::Long);
                    } else if (currentWord == "Text") {
                        fieldTypes.push_back(FieldType::Text);
                    } else {
                        throw IOException("FileInOut::readEntryInfo: wrong field types in file.");
                    }
                } else {
                    fieldsNames.push_back(currentWord);
                }
                currentWord = "";
            }
            if (currChar == '\n') {
                if (currentLine == 1) {
                    fieldsNames.push_back(currentWord);
                    currentLine = 2;
                    break;
                }
                if (currentWord == "Long") {
                    fieldTypes.push_back(FieldType::Long);
                } else if (currentWord == "Text") {
                    fieldTypes.push_back(FieldType::Text);
                } else {
                    throw IOException("FileInOut::readEntryInfo: wrong field types in file.");
                }
                currentWord = "";
                currentLine = 1;
            }
        }
        if (currentLine == 2) {
            break;
        }
        readedSize = read(fileDesc, buffer, BUFFER_SIZE);
    }
    if (currentLine != 2) {
        throw IOException("FileInOut::readEntryInfo: no information about table in file.");
    }
    return Factory::createEntry(fieldTypes, fieldsNames);
}

void FileInOut::readEntry(int fileDesc, Entry *entry, size_t line) {
    if (seekToEntry(fileDesc, line) == -1) {
        throw  IOException("FileInOut::readEntry: wrong position.");
    };
    char buffer[BUFFER_SIZE];
    std::string currentWord = "";
    char currChar;
    size_t readedSize, currIndex = 0;
    bool endFlag = false;
    readedSize = read(fileDesc, buffer, BUFFER_SIZE);
    while (readedSize && !endFlag) {
        for (size_t i = 0; i < readedSize; i++) {
            currChar = buffer[i];
            if ((currChar != ' ') && (currChar != '\n')) {

                currentWord += currChar;
            } else {
                if (entry->fields[currIndex]->getType() == "Long") {
                    dynamic_cast<LongField *>(entry->fields[currIndex])->number = atol(currentWord.c_str());
                } else if (entry->fields[currIndex]->getType() == "Text") {
                    dynamic_cast<TextField *>(entry->fields[currIndex])->text = currentWord;
                } else {
                    throw IOException("FileInOut::readEntry: no information about entry.");
                }
                currentWord = "";
                currIndex++;
                if (currChar == '\n') {
                    endFlag = true;
                    break;
                }
            }
        }
        readedSize = read(fileDesc, buffer, BUFFER_SIZE);
    }
    if (!endFlag) {
        throw IOException("FileInOut::readEntry: entry information is corrupted.");
    }
}

void FileInOut::writeEntry(int fileDesc, Entry *entry) {
    std::string str = "";
    for (size_t i = 0; i < entry->fields.size(); i++) {
        str += entry->fields[i]->getText();
        if (i != entry->fields.size() - 1) {
            str += " ";
        }
    }
    str += "\n";
    if (write(fileDesc, str.c_str(), str.size()) <= 0) {
        throw IOException("FileInOut::writeEntry: write corrupted");
    }; //TODO
}

void FileInOut::writeEntryInfo(int fileDesc, Entry *entry) {
    lseek(fileDesc, 0, SEEK_SET);
    std::string str = "";
    for (size_t i = 0; i < entry->fields.size(); i++) {
        str += entry->fields[i]->getType();
        if (i != entry->fields.size() - 1) {
            str += " ";
        }
    }
    str += "\n";
    if (write(fileDesc, str.c_str(), str.size()) < 0) {
        throw IOException("FileInOut::writeEntryInfo: write corrupted");
    };
    str = "";
    for (size_t i = 0; i < entry->fields.size(); i++) {
        str += entry->fields[i]->getFieldName();
        if (i != entry->fields.size() - 1) {
            str += " ";
        }
    }
    str += "\n";
    if (write(fileDesc, str.c_str(), str.size()) <= 0) {
        throw IOException("FileInOut::writeEntryInfo: write corrupted");
    }; //TODO
}


int FileInOut::seekToEntry(int fileDesc, size_t line) {
    lseek(fileDesc, 0, SEEK_SET);
    char buffer[BUFFER_SIZE];
    char currChar;
    size_t readedSize, currSeek = 0, currLine = 0;
    if ((readedSize = read(fileDesc, buffer, BUFFER_SIZE)) == 0) {
        throw IOException("FileInOut::seekToEntry: file is empty");
    }
    while (readedSize && (currLine < line + 2)) {
        for (size_t i = 0; i < readedSize; ++i) {
            currSeek++;
            currChar = buffer[i];
            if (currChar == '\n') {
                currLine++;
                if (currLine == line + 2) {

                    break;
                }
            }
        }
        readedSize = read(fileDesc, buffer, BUFFER_SIZE);
    }
    if (currLine != line + 2 ) {
        return -1;
    }

    if (lseek(fileDesc, 0, SEEK_END) == (int)currSeek) {
        return -1;
    }
    lseek(fileDesc, currSeek, SEEK_SET) ;
    return currSeek;
}

void FileInOut::deleteEntry(int fileDesc, const std::string &fileName, size_t line) {
    lseek(fileDesc, 0, SEEK_SET);
    char buffer[BUFFER_SIZE];
    char currChar;
    size_t readedSize, deletedSize = 0, currLine = 0;
    std::string tmpName = tmpnam(nullptr);
    int tmp = open(tmpName.c_str(), O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    std::string currentWord = "";
    readedSize = read(fileDesc, buffer, BUFFER_SIZE);
    while (readedSize) { //find line length
        for (size_t i = 0; i < readedSize; ++i) {
            deletedSize++;
            currChar = buffer[i];
            currentWord += currChar;
            if (currChar == '\n') {
                currLine++;
                if (currLine != (line + 3)) {
                    if (write(tmp, currentWord.c_str(), currentWord.size()) <= 0) {
                        throw IOException("FileInOut::deleteEntry: write corrupted");
                    }
                }

                currentWord = "";
            }
        }
        readedSize = read(fileDesc, buffer, BUFFER_SIZE);
    }
    lseek(tmp, 0, SEEK_SET);
    lseek(fileDesc, 0, SEEK_SET);
    if (truncate(fileName.c_str(), 0)) {};
    readedSize = read(tmp, buffer, BUFFER_SIZE);
    while (readedSize) { //find line length
        for (size_t i = 0; i < readedSize; ++i) {
            deletedSize++;
            currChar = buffer[i];
            currentWord += currChar;
            if (currChar == '\n') {
                if (write(fileDesc, currentWord.c_str(), currentWord.size()) <= 0) {
                    throw IOException("FileInOut::deleteEntry: write corrupted");
                };
                currentWord = "";
            }
        }
        readedSize = read(tmp, buffer, BUFFER_SIZE);
    }
    unlink(tmpName.c_str());
}
