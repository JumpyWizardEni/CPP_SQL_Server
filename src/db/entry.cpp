#include <stdexcept>
#include "entry.h"
#include "../exceptions.h"

Entry *Entry::setField(FieldType fieldType, const std::string &fieldName) {
    switch (fieldType) {
        case FieldType::Long:
            fields.push_back(new LongField(fieldName));
            break;
        case FieldType::Text:
            fields.push_back(new TextField(fieldName));
            break;
        default:
            throw TableCreatingException("Entry::setField: wrong field type.");//TODO()
            break;
    }
    return this;
}

Entry::~Entry() {
    for (auto &field : fields) {
        delete field;
    }
}

TextField::TextField(const std::string &newFieldName) {
    fieldName = newFieldName;
}

std::string TextField::getType() {
    return "Text";
}

std::string TextField::getText() {
    return text;
}

long TextField::getLong() {
    return 0;
}

std::string TextField::getFieldName() {
    return fieldName;
}

FieldType TextField::getTypeEnum() {
    return FieldType::Text;
}

LongField::LongField(const std::string &newFieldName) {
    fieldName = newFieldName;
    number = 0;
}

std::string LongField::getType() {
    return "Long";
}

std::string LongField::getText() {
    std::stringstream stream;
    std::string str;
    stream << number;
    stream >> str;
    return str;
}

long LongField::getLong() {
    return number;
}

std::string LongField::getFieldName() {
    return fieldName;
}

FieldType LongField::getTypeEnum() {
    return FieldType::Long;
}

Field::~Field() = default;
