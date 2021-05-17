#include "dbInterpreter.h"

std::string DBInterpreter::parseRequest(const char *request) {
    std::stringstream stream;
    stream << request;
    parser = Parser(&stream);
    serverAnswer = "";
    parser.parse();
    switch (parser.answer.operation) {
        case LEX_SELECT:
            select();
            break;
        case LEX_INSERT:
            insert();
            break;
        case LEX_UPDATE:
            update();
            break;
        case LEX_DELETE:
            delete_();
            break;
        case LEX_CREATE:
            create();
            break;
        case LEX_DROP:
            drop();
            break;
        default:
            throw SyntaxException("request must start with SELECT, INSERT, UPDATE, DELETE, CREATE or DROP");
            break;//TODO
    }
    return serverAnswer;
}

void DBInterpreter::select() {
    Entry *entry;
    std::vector<size_t> indices;
    bool found = false;
    SQLTable *table = Factory::openTable(parser.answer.tableName);
    entry = table->readEntryInfo();
    if (parser.answer.fields[0].getType() == LEX_STAR) {
        indices.resize(entry->fields.size());
        for (size_t i = 0; i < entry->fields.size(); ++i) {
            indices[i] = i;
        }
    } else {
        for (size_t i = 0; i < parser.answer.fields.size(); ++i) {
            found = false;
            for (size_t j = 0; j < entry->fields.size(); ++j) {
                if (parser.answer.fields[i].getValue() == entry->fields[j]->getFieldName()) {
                    found = true;
                    indices.push_back(i);
                    break;
                }
            }
            if (!found) {
                throw SemanticException("wrong SELECT fields: no such fields in table");
            }
        }
        std::sort(indices.begin(), indices.end());
    }

    for (auto i = indices.begin(); i != indices.end(); ++i) {
        serverAnswer += entry->fields[*i]->getFieldName();
        serverAnswer += " ";
    }
    serverAnswer += "\n";

    while (table->getNextEntry(entry) >= 0) {
        if (isEntryAcceptable(entry, WHERE)) {
            addEntryToAnswer(entry, indices);
        }
    }
}

void DBInterpreter::insert() {
    SQLTable *table = Factory::openTable(parser.answer.tableName);
    Entry *newEntry = table->readEntryInfo();
    if (parser.answer.fields.size() != newEntry->fields.size()) {
        throw SemanticException("wrong INSERT fields: fields must repeat table structure");
        return;
    }
    for (size_t i = 0; i < newEntry->fields.size(); ++i) {
        if (newEntry->fields[i]->getType() == "Long") {
            if (parser.answer.fields[i].getType() != LEX_NUMBER) {
                throw SemanticException("wrong INSERT field type");
            }
            dynamic_cast<LongField *>(newEntry->fields[i])->number = atol(
                    parser.answer.fields[i].getValue().c_str());
        } else if (newEntry->fields[i]->getType() == "Text") {
            if (parser.answer.fields[i].getType() != LEX_STRING) {
                throw SemanticException("wrong INSERT field type");
            }
            dynamic_cast<TextField *>(newEntry->fields[i])->text = parser.answer.fields[i]
                    .getValue().substr(1,
                                       parser.answer.fields[i].getValue().length() -
                                       2);
        } else {
            throw SemanticException("wrong INSERT field type");
        }
    }
    table->addEntry(newEntry);
    serverAnswer = "Success: entry added";
}


void DBInterpreter::update() {
    Entry *entry;
    SQLTable *table = Factory::openTable(parser.answer.tableName);
    entry = table->readEntryInfo();
    int position = 0;
    size_t i = 0;
    TypeOfLex fieldType = LEX_NULL;
    for (i = 0; i < entry->fields.size(); ++i) {
        if (entry->fields[i]->getFieldName() == parser.answer.fields[0].getValue()) {
            if (parser.answer.polizStack.size() == 1 && ((parser.answer.polizStack.front().getType() == LEX_STRING) ||
                                                         (parser.answer.polizStack.front().getType() == LEX_STRING &&
                                                          getFieldType(entry,
                                                                       parser.answer.polizStack.front().getValue()) ==
                                                          "Text"))) {
                fieldType = LEX_STRING;
                if (entry->fields[i]->getType() != "Text") {
                    throw SemanticException("wrong UPDATE field type");
                }
            } else {
                fieldType = LEX_NUMBER;
                if (entry->fields[i]->getType() != "Long") {
                    throw SemanticException("wrong UPDATE field type");
                }
            }
            break;
        }
    }
    if (i == entry->fields.size()) {
        throw SemanticException("UPDATE request: no such field in table");
    }
    int number = 0;
    if (fieldType == LEX_NUMBER) {
        number = countLongExpression(entry, true, UPDATE);
    }
    while ((position = table->getNextEntry(entry)) >= 0) {
        if (isEntryAcceptable(entry, UPDATE)) {
            if (fieldType == LEX_STRING) {
                if (dynamic_cast<TextField *>(entry->fields[i])->text == parser.answer.polizStack.front().getValue()) {
                    continue;
                }
                dynamic_cast<TextField *>(entry->fields[i])->text = parser.answer.polizStack.front().getValue();
            } else {
                if (dynamic_cast<LongField *>(entry->fields[i])->number == number) {
                    continue;
                }
                dynamic_cast<LongField *>(entry->fields[i])->number = number;
            }
            table->updateEntry(position, entry);
        }
    }
    serverAnswer = "Success: entries were updated";

}

void DBInterpreter::delete_() {
    Entry *entry;
    SQLTable *table = Factory::openTable(parser.answer.tableName);
    entry = table->readEntryInfo();
    int position = 0;
    while ((position = table->getNextEntry(entry)) >= 0) {
        if (isEntryAcceptable(entry, WHERE)) {
            table->deleteEntry(position);
        }
    }
    serverAnswer = "Success: entries were deleted";
}

void DBInterpreter::create() {
    try {
        std::vector<FieldType> fieldTypes;
        std::vector<std::string> fieldNames;
        for (size_t i = 0; i < parser.answer.fields.size(); ++i) {
            fieldTypes.push_back(stringToType(parser.answer.types[i]));
            fieldNames.push_back(parser.answer.fields[i].getValue());
        }
        Entry *entry = Factory::createEntry(fieldTypes, fieldNames);
        Factory::createTable(parser.answer.tableName, entry);
        serverAnswer = "Success: table created";
    } catch (const SQLException &e) {
        serverAnswer = e.getMessage();
    }

}

void DBInterpreter::drop() {
    try {
        Factory::dropTable(parser.answer.tableName);
        serverAnswer = "Success: table deleted";
    } catch (const TableCreatingException &exception) {
        serverAnswer = exception.getMessage();
    }
}

FieldType DBInterpreter::stringToType(std::string &string) {
    if (string == "Text") {
        return FieldType::Text;
    } else if (string == "Long") {
        return FieldType::Long;
    } else {
        throw TableEditingException("table corrupting");
    }
}

bool DBInterpreter::isEntryAcceptable(Entry *entry, Mode mode) {
    bool result = true;
    if (parser.answer.whereOperationStack.size() == 0) {
        if (!countBoolExpression(entry, mode)) {
            result = false;
        }
    } else if (parser.answer.whereOperationStack.top() == LEX_ALL) {
        return true;
    } else if (parser.answer.whereOperationStack.top() == LEX_LIKE) {
        bool entryFound = false;
        std::string fieldType = "Long", checkString = parser.answer.polizStack.front().getValue();
        std::string sampleString = parser.answer.filterVector[0].getValue();
        size_t position = 0;
        for (position = 0; position < entry->fields.size(); ++position) {
            if (entry->fields[position]->getFieldName() == parser.answer.polizStack.front().getValue()) {
                fieldType = entry->fields[position]->getType();
                entryFound = true;
                break;
            }
        }
        if (!entryFound || fieldType != "Text") {
            throw SemanticException("wrong WHERE field type");
        }
        result = checkSample(entry->fields[position]->getText(), sampleString);
    } else if (parser.answer.whereOperationStack.top() == LEX_IN) {
        TypeOfLex typeOfLex = parser.answer.polizStack.front().getType();
        if (parser.answer.polizStack.size() == 1 && (typeOfLex == LEX_STRING)) {
            bool containsFlag = false;
            for (size_t i = 0; i < parser.answer.filterVector.size(); ++i) {
                if (parser.answer.filterVector[i].getType() == LEX_STRING &&
                    parser.answer.filterVector[i].getValue() == parser.answer.polizStack.front().getValue()) {
                    containsFlag = true;
                    break;
                }
            }
            if (!containsFlag) {
                result = false;
            }
        } else if ((parser.answer.polizStack.size() == 1) && (typeOfLex == LEX_IDENT)) {
            bool containsFlag = false;
            typeOfLex = LEX_NULL;
            std::string value = "";
            for (size_t i = 0; i < entry->fields.size(); ++i) {
                if (parser.answer.polizStack.front().getValue() == entry->fields[i]->getFieldName()) {
                    value = entry->fields[i]->getText();
                    if (entry->fields[i]->getType() == "Long") {
                        typeOfLex = LEX_NUMBER;
                    } else {
                        typeOfLex = LEX_STRING;
                    }
                    break;
                }
            }
            if (typeOfLex == LEX_NULL) {
                throw SemanticException("wrong WHERE field type");
            }
            for (size_t i = 0; i < parser.answer.filterVector.size(); ++i) {
                if (parser.answer.filterVector[i].getType() == typeOfLex &&
                    parser.answer.filterVector[i].getValue() == value) {
                    containsFlag = true;
                    break;
                }
            }
            if (!containsFlag) {
                result = false;
            }

        } else {
            int longResult = countLongExpression(entry, false, WHERE);
            if (!isContains(longResult)) {
                result = false;
            }
        }

    }
    return parser.answer.whereOperationStack.size() == 2 ? !result : result; // NOT IN, NOT LIKE
}

int DBInterpreter::countLongExpression(Entry *entry, bool isUpdateFlag, Mode mode) {
    std::stack<int> result;
    std::deque<Lex> poliz = parser.answer.polizStack;
    if (mode == UPDATE && !isUpdateFlag) {
        while (poliz.size() > 0 && poliz.front().getType() != LEX_POLIZ_END) {
            poliz.pop_front();
        }
        if (poliz.size() > 0) {
            poliz.pop_front();
        }
    }
    while (poliz.size() > 0 && poliz.front().getType() != LEX_POLIZ_END) {
        Lex currentLex = poliz.front();
        poliz.pop_front();
        TypeOfLex currentLexType = currentLex.getType();
        if (currentLexType == LEX_NUMBER) {
            result.push(atol(currentLex.getValue().c_str()));
        } else if (currentLexType == LEX_IDENT) {
            bool found = false;
            for (size_t i = 0; i < entry->fields.size(); ++i) {
                if (entry->fields[i]->getFieldName() == currentLex.getValue()) {
                    if (entry->fields[i]->getType() != "Long") {
                        throw SemanticException("wrong WHERE field type");
                    }
                    found = true;
                    result.push(entry->fields[i]->getLong());
                    break;
                }
            }
            if (!found) {
                throw SemanticException("WHERE: field not found");
            }

        } else { // бинарные операции
            int newResult = 0;
            int second = result.top();
            result.pop();
            int first = result.top();
            result.pop();
            switch (currentLexType) {
                case LEX_PLUS:
                    newResult = first + second;
                    break;
                case LEX_MINUS:
                    newResult = first - second;
                    break;
                case LEX_STAR:
                    newResult = first * second;
                    break;
                case LEX_DIV:
                    newResult = first / second;
                    break;
                case LEX_MOD:
                    newResult = first % second;
                    break;
                default:
                    throw SemanticException("wrong operation");
            }
            result.push(newResult);
        }
    }
    return result.top();
}

bool DBInterpreter::checkSample(const std::string &checkString, const std::string &sample) {
    std::string newSample = "";
    for (size_t i = 0; i < sample.size(); ++i) {
        if (sample[i] == '%') {
            newSample += ".*";
        } else if (sample[i] == '_') {
            newSample += ".";
        } else if (sample[i] == '\'') {
            continue;
        } else {
            newSample += sample[i];
        }
    }
    const std::regex reg(newSample);
    return std::regex_match(checkString, reg);
}

DBInterpreter::DBInterpreter() {
    parser = Parser();
    serverAnswer = "";
}

bool DBInterpreter::isContains(int number) {
    for (size_t i = 0; i < parser.answer.filterVector.size(); ++i) {
        if (parser.answer.filterVector[i].getType() == LEX_NUMBER) {
            if (atol(parser.answer.filterVector[i].getValue().c_str()) == number) {
                return true;
            }
        }
    }
    return false;
}

bool DBInterpreter::countBoolExpression(Entry *entry, Mode mode) {
    std::stack<Lex> result;
    std::deque<Lex> poliz = parser.answer.polizStack;
    if (mode == UPDATE) {
        while (poliz.size() > 0 && poliz.front().getType() != LEX_POLIZ_END) {
            poliz.pop_front();
        }
        if (poliz.size() > 0) {
            poliz.pop_front();
        }
    }
    while (poliz.size() > 0) {
        Lex currentLex = poliz.front();
        poliz.pop_front();
        TypeOfLex currentLexType = currentLex.getType();
        if (currentLexType == LEX_NUMBER || currentLexType == LEX_STRING) {
            result.push(currentLex);
        } else if (currentLexType == LEX_IDENT) {
            bool found = false;
            for (size_t i = 0; i < entry->fields.size(); ++i) {
                if (entry->fields[i]->getFieldName() == currentLex.getValue()) {
                    found = true;
                    currentLexType = entry->fields[i]->getType() == "Long" ? LEX_NUMBER : LEX_STRING;
                    std::string oldString = entry->fields[i]->getText();
                    if (currentLexType == LEX_STRING) {
                        std::string newString = "\'";
                        for (size_t j = 0; j < oldString.size(); ++j) {
                            newString += oldString[j];
                        }
                        newString += "\'";
                        result.push(Lex(currentLexType, newString));
                    } else {
                        result.push(Lex(currentLexType, oldString));
                    }

                    break;
                }
            }
            if (!found) {
                throw SemanticException("WHERE: field not found");
            }

        } else { // бинарные операции
            bool newResult = false;
            Lex second = result.top();
            result.pop();
            if (currentLexType == LEX_NOT) {
                std::stringstream stream;
                stream << second.getValue();
                bool s;
                stream >> s;
                std::string newValue;
                s = !s;
                stream << s;
                stream >> newValue;
                result.push(Lex(LEX_BOOL, newValue));
                continue;
            }
            Lex first = result.top();
            result.pop();
            if (first.getType() != second.getType()) {
                throw SemanticException("wrong type");
            }
            switch (currentLexType) {
                case LEX_AND: {
                    bool f, s;
                    std::stringstream stream1, stream2;
                    stream1 << first.getValue();
                    stream1 >> f;
                    stream2 << second.getValue();
                    stream2 >> s;
                    newResult = f && s;
                }
                    break;
                case LEX_OR: {
                    bool f, s;
                    std::stringstream stream1, stream2;
                    stream1 << first.getValue();
                    stream1 >> f;
                    stream2 << second.getValue();
                    stream2 >> s;
                    newResult = f || s;
                }
                    break;
                case LEX_EQUAL:
                    newResult = first.getValue() == second.getValue();
                    break;
                case LEX_NOT_EQUAL:
                    newResult = first.getValue() != second.getValue();
                    break;
                case LEX_LOWER:
                    if (first.getType() == LEX_NUMBER) {
                        newResult = atol(first.getValue().c_str()) < atol(second.getValue().c_str());
                    } else {
                        newResult = first.getValue() < second.getValue();
                    }
                    break;
                case LEX_LOWER_EQUAL:
                    if (first.getType() == LEX_NUMBER) {
                        newResult = atol(first.getValue().c_str()) <= atol(second.getValue().c_str());
                    } else {
                        newResult = first.getValue() <= second.getValue();
                    }
                    break;
                case LEX_GREATER:
                    if (first.getType() == LEX_NUMBER) {
                        newResult = atol(first.getValue().c_str()) > atol(second.getValue().c_str());
                    } else {
                        newResult = first.getValue() > second.getValue();
                    }
                    break;
                case LEX_GREATER_EQUAL:
                    if (first.getType() == LEX_NUMBER) {
                        newResult = atol(first.getValue().c_str()) >= atol(second.getValue().c_str());
                    } else {
                        newResult = first.getValue() >= second.getValue();
                    }
                    break;
                default:
                    throw SemanticException("wrong bool operation");
                    break;
            }
            std::stringstream stream;
            stream << newResult;
            std::string newResString;
            stream >> newResString;
            result.push(Lex(LEX_BOOL, newResString));
        }
    }
    std::stringstream stream;
    stream << result.top().getValue();
    bool returnValue;
    stream >> returnValue;
    return returnValue;
}

std::string DBInterpreter::getFieldType(Entry *entry, const std::string &fieldName) {
    for (size_t i = 0; i < entry->fields.size(); ++i) {
        if (fieldName == entry->fields[i]->getFieldName()) {
            return entry->fields[i]->getType();
        }
    }
    throw SemanticException("field not found");
}

void DBInterpreter::addEntryToAnswer(Entry *entry, std::vector<size_t> &indices) {
    for (auto i = indices.begin(); i != indices.end(); ++i) {
        serverAnswer += entry->fields[*i]->getText();
        serverAnswer += " ";
    }
    serverAnswer += "\n";
}


