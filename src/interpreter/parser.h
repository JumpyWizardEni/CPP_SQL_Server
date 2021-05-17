#ifndef SQL_P3_PARSER_H
#define SQL_P3_PARSER_H
#include "scanner.h"
#include <stack>
#include "../db/entry.h"
#include <iostream>

struct ParserAnswer {
    std::string tableName;
    std::vector<Lex> fields;
    std::vector<std::string> types;
    std::vector<Lex> filterVector;
    std::deque<Lex> polizStack;
    std::stack<TypeOfLex>  whereOperationStack;
    TypeOfLex operation;
    ParserAnswer();
};

class Parser {
private:

    std::stack<TypeOfLex> typeStack;
    std::deque<TypeOfLex> typeCheck;
    std::stack<Lex> expressionStack;
    Lex currentLex;
    TypeOfLex currentLexType;
    Scanner scanner;
    void getLex();
    void START();
    void SELECT();
    void INSERT();
    void UPDATE();
    void DELETE();
    void CREATE();
    void DROP();
    void WHERE();
    void WHERE_EXPRESSION();
    void CONST_SET();
    void STRING();
    void NUMBER();
    void IDENT();
    void LONG_BOOL_EXPRESSION();
    void SUMMAND();
    void MULTIPLIER();
    void OR_OP();
    void AND_OP();
    void NOT_OP();
    void COMPARE_OP();
    void DESCRIPTION_SET();
    void DESCRIPTION();
    void FIELD_TYPE();
    void NAME();
    void TABLE_NAME();
    void FIELDS();
    void FIELD_VALUE();
    void checkTypes();
public:
    ParserAnswer answer;
    Parser(std::stringstream *stream);
    ParserAnswer & parse();
    Parser();
};


#endif //SQL_P3_PARSER_H
