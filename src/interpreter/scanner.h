#ifndef SQL_P3_SCANNER_H
#define SQL_P3_SCANNER_H
#include <string>
#include <vector>
#include <sstream>
#include "../exceptions.h"
enum TypeOfLex {
    LEX_NULL,
    LEX_POLIZ_END,
    LEX_BOOL,
    LEX_NUMBER,
    LEX_IDENT,
    LEX_STRING,

    LEX_EQUAL,
    LEX_GREATER,
    LEX_LOWER,
    LEX_GREATER_EQUAL,
    LEX_LOWER_EQUAL,
    LEX_NOT_EQUAL,
    LEX_PLUS,
    LEX_MINUS,
    LEX_STAR,
    LEX_DIV,
    LEX_MOD,
    LEX_LEFT_PAR,
    LEX_RIGHT_PAR,
    LEX_COMMA,

    LEX_NOT,
    LEX_OR,
    LEX_AND,
    LEX_LIKE,
    LEX_WHERE,
    LEX_ALL,
    LEX_IN,
    LEX_DROP,
    LEX_TABLE,
    LEX_TEXT,
    LEX_LONG,
    LEX_CREATE,
    LEX_DELETE,
    LEX_FROM,
    LEX_UPDATE,
    LEX_SET,
    LEX_SELECT,
    LEX_INTO,
    LEX_INSERT
};

class Lex {
    TypeOfLex lexType;
    std::string lexValue;
public:
    Lex(TypeOfLex type, std::string value);

    Lex();

    TypeOfLex getType();

    std::string getValue();

    friend std::ostream &operator<<(std::ostream &s, Lex l);
};


class Scanner {
    enum state {
        IDENT, NUMBER, DELIM, STRING, H
    };
    state CS;
    int currentChar;
    std::stringstream *file;
    std::vector <std::string> ident;
    std::vector <TypeOfLex> reservedWordsTypes = {LEX_NOT, LEX_OR, LEX_AND, LEX_LIKE, LEX_WHERE, LEX_ALL, LEX_IN,
                                                  LEX_DROP, LEX_TABLE, LEX_TEXT, LEX_LONG, LEX_CREATE, LEX_DELETE,
                                                  LEX_FROM, LEX_UPDATE, LEX_SET, LEX_SELECT, LEX_INTO, LEX_INSERT};
    std::vector <std::string> reservedWords = {"NOT", "OR", "AND", "LIKE", "WHERE", "ALL", "IN", "DROP", "TABLE",
                                               "TEXT", "LONG", "CREATE", "DELETE", "FROM", "UPDATE", "SET", "SELECT",
                                               "INTO", "INSERT"};
    std::vector <TypeOfLex> delimitersTypes = {LEX_PLUS, LEX_MINUS, LEX_STAR, LEX_DIV, LEX_MOD, LEX_COMMA,
                                               LEX_LEFT_PAR, LEX_RIGHT_PAR, LEX_EQUAL, LEX_GREATER, LEX_LOWER,
                                               LEX_GREATER_EQUAL, LEX_LOWER_EQUAL, LEX_NOT_EQUAL};
    std::vector <std::string> delimiters = {"+", "-", "*", "/", "%", ",", "(", ")", "=", ">", "<", ">=", "<=", "!="};
public:
    Scanner(std::stringstream *f);

    Scanner();

    Lex getLex();

    size_t findInWords(std::string);

    size_t findInDelim(std::string);

    void gc();

};

#endif //SQL_P3_SCANNER_H
