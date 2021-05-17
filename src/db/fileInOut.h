#ifndef SQL_P3_FILEINOUT_H
#define SQL_P3_FILEINOUT_H
#include <string>
#include "entry.h"
#define BUFFER_SIZE 4096

class FileInOut {
public:
    static Entry * readEntryInfo(int fileDesc);
    static void readEntry(int fileDesc, Entry *entry, size_t line);
    static void writeEntry(int fileDesc, Entry *);
    static void writeEntryInfo(int fileDesc, Entry *);
    static void deleteEntry(int fileDesc,  const std::string &fileName, size_t line);
    static int seekToEntry(int fileDesc, size_t line);
};


#endif //SQL_P3_FILEINOUT_H
