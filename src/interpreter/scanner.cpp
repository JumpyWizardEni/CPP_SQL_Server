#include "scanner.h"

Lex::Lex(TypeOfLex type, std::string value) {
    lexType = type;
    lexValue = value;
}

TypeOfLex Lex::getType() {
    return lexType;
}

std::string Lex::getValue() {
    return lexValue;
}

std::ostream &operator<<(std::ostream &s, Lex l) {
    s << "Type: " << l.getType() << ", Value: " << l.getValue() << std::endl;
    return s;
}

Lex::Lex() {

}

Scanner::Scanner(std::stringstream *f) {
    file = f;
    gc();
}

void Scanner::gc() {
    currentChar = file->get();
}

size_t Scanner::findInWords(std::string checkStr) {
    for (size_t i = 0; i < reservedWords.size(); ++i) {
        if (reservedWords[i] == checkStr) {
            return i + 1;
        }
    }
    return 0;
}

size_t Scanner::findInDelim(std::string checkStr) {
    for (size_t i = 0; i < delimiters.size(); ++i) {
        if (delimiters[i] == checkStr) {
            return i + 1;
        }
    }
    return 0;
}

Lex Scanner::getLex() {
    size_t index;
    CS = H;
    std::string lex = "";
    do {
        switch (CS) {
            case H:
                if (isspace(currentChar)) {
                    gc();
                    continue;
                } else if (currentChar == '_' || isalpha(currentChar)) {
                    CS = IDENT;
                } else if (currentChar == '0') {
                    return Lex(LEX_NUMBER, "0");
                } else if (isdigit(currentChar)) {
                    CS = NUMBER;
                } else if (currentChar == '\'') {
                    CS = STRING;
                } else if (currentChar == '<' || currentChar == '>' || currentChar == '=' || currentChar == '!' ||
                           currentChar == '+' ||
                           currentChar == '-' || currentChar == '*' || currentChar == '/' || currentChar == '%' ||
                           currentChar == ',' || currentChar == '(' || currentChar == ')') {
                    CS = DELIM;
                } else if (currentChar == EOF) {
                    return Lex(LEX_NULL, "0");
                } else {
                    throw LexException("wrong lex");
                }
                lex += currentChar;
                gc();
                break;
            case IDENT:
                if (isalpha(currentChar) || isdigit(currentChar) || currentChar == '_') {
                    lex += currentChar;
                    gc();
                } else if ((index = findInWords(lex)))
                    return Lex(reservedWordsTypes[index - 1], lex);
                else {
                    bool containsFlag = false;
                    for (size_t i = 0; i < ident.size(); ++i) {
                        if (ident[i] == lex) {
                            containsFlag = true;
                            break;
                        }
                    }
                    if (!containsFlag) {
                        ident.push_back(lex);
                    }
                    return Lex(LEX_IDENT, lex);
                }
                break;
            case NUMBER:
                if (isdigit(currentChar)) {
                    lex += currentChar;
                    gc();
                } else
                    return Lex(LEX_NUMBER, lex);
                break;
            case DELIM: // < = > <= >= !=
                if (currentChar == '=') {
                    lex += currentChar;
                    gc();
                }
                if ((index = findInDelim(lex))) {
                    return Lex(delimitersTypes[index - 1], delimiters[index - 1]);
                } else {
                    throw LexException("wrong delimiter");
                }
                break;

            case STRING:
                if (currentChar == EOF) {
                    throw LexException("unexpected EOF");
                }
                if (currentChar == '\'') {
                    lex += currentChar;
                    gc();
                    return Lex(LEX_STRING, lex);
                } else {
                    lex += currentChar;
                    gc();
                }
                break;
            default:
                throw LexException("wrong lex in request");
                break;
        }
    } while (true);
}

Scanner::Scanner() {

}

