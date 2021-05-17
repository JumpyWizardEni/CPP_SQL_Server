#ifndef SQL_P3_ENTRY_H
#define SQL_P3_ENTRY_H

#include <vector>
#include <string>
#include <sstream>

enum FieldType {
    Text, Long
};

struct Field {
    virtual FieldType getTypeEnum() = 0;
    virtual std::string getType() = 0;

    virtual std::string getFieldName() = 0;

    virtual std::string getText() = 0;

    virtual long getLong() = 0;

    virtual ~Field();
};

struct TextField : public Field {
    std::string fieldName;
    std::string text;
    explicit TextField(const std::string &newFieldName);
    FieldType getTypeEnum() override;
    std::string getType() override;
    std::string getFieldName() override;
    std::string getText() override;

    long getLong() override;

};

struct LongField : public Field {
    std::string fieldName;
    long number;
    explicit LongField(const std::string &newFieldName);
    FieldType getTypeEnum() override;
    std::string getType() override;
    std::string getFieldName() override;
    std::string getText() override;

    long getLong() override;
};


struct Entry {
    Entry *setField(FieldType, const std::string &fieldName);

    ~Entry();

    std::vector<Field *> fields;
};

#endif