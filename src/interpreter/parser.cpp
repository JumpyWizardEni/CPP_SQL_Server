#include "parser.h"


void Parser::getLex() {
    currentLex = scanner.getLex();
    currentLexType = currentLex.getType();
}

ParserAnswer &Parser::parse() {
    getLex();
    START();
    if (currentLexType != LEX_NULL) {
        throw SyntaxException("expected LEX_NULL at the end of the request");
    }
    checkTypes();
    return this->answer;
}

void Parser::START() {
    answer.operation = currentLexType;
    switch (currentLexType) {
        case LEX_SELECT:
            getLex();
            SELECT();
            break;
        case LEX_INSERT:
            getLex();
            INSERT();
            break;
        case LEX_UPDATE:
            getLex();
            UPDATE();
            break;
        case LEX_DELETE:
            getLex();
            DELETE();
            break;
        case LEX_CREATE:
            getLex();
            CREATE();
            break;
        case LEX_DROP:
            getLex();
            DROP();
            break;
        default:
            throw SyntaxException("request must start with SELECT, INSERT, UPDATE, DELETE, CREATE or DROP");
            break;
    }
    while (expressionStack.size() != 0) {
        answer.polizStack.push_back(expressionStack.top());
        expressionStack.pop();

    }
    while (typeStack.size() != 0) {
        typeCheck.push_back(typeStack.top());
        typeStack.pop();
    }

}

void Parser::SELECT() {
    FIELDS();
    if (currentLexType != LEX_FROM) {
        throw SyntaxException("FROM must go after SELECT");
    }
    getLex();
    TABLE_NAME();
    WHERE();
}


void Parser::NAME() {
    if (currentLexType != LEX_IDENT) {
        throw SyntaxException("wrong name");
    }
    answer.fields.push_back(currentLex);
    getLex();
}

void Parser::TABLE_NAME() {
    if (currentLexType != LEX_IDENT) {
        throw SyntaxException("wrong table name");
    }
    answer.tableName = currentLex.getValue();
    getLex();
}


void Parser::FIELDS() {
    if (currentLexType == LEX_STAR) {
        answer.fields.push_back(currentLex);
        getLex();
        return;
    }
    NAME();
    while (currentLexType == LEX_COMMA) {
        getLex();
        NAME();
    }

}

void Parser::INSERT() {
    if (currentLexType != LEX_INTO) {
        throw SyntaxException("INTO must go after INSERT");
    }
    getLex();
    TABLE_NAME();
    if (currentLexType != LEX_LEFT_PAR) {
        throw SyntaxException("left parenthesis must go after table name in INSERT request");
    }
    getLex();
    FIELD_VALUE();
    while (currentLexType == LEX_COMMA) {
        getLex();
        FIELD_VALUE();
    }
    if (currentLexType != LEX_RIGHT_PAR) {
        throw SyntaxException("INSERT request must must end with right parenthesis");
    }
    getLex();
}

void Parser::FIELD_VALUE() {
    if (currentLexType != LEX_NUMBER && currentLexType != LEX_STRING) {
        throw SyntaxException("wrong field value");
    }
    answer.fields.push_back(currentLex);
    getLex();
}

void Parser::UPDATE() {
    TABLE_NAME();
    if (currentLexType != LEX_SET) {
        throw SyntaxException("SET must go after UPDATE");
    }
    getLex();
    NAME();
    if (currentLexType != LEX_EQUAL) {
        throw SyntaxException("\'=\' must go after name in UPDATE request");
    }
    getLex();
    LONG_BOOL_EXPRESSION();
    while (expressionStack.size() != 0) {
        answer.polizStack.push_back(expressionStack.top());
        expressionStack.pop();

    }
    while (typeStack.size() != 0) {
        typeCheck.push_back(typeStack.top());
        typeStack.pop();
    }
    answer.polizStack.push_back(Lex(LEX_POLIZ_END, "0"));
    WHERE();
}

void Parser::DELETE() {
    if (currentLexType != LEX_FROM) {
        throw SyntaxException("FROM must go after DELETE");
    }
    getLex();
    TABLE_NAME();
    WHERE();
}

void Parser::CREATE() {
    if (currentLexType != LEX_TABLE) {
        throw SyntaxException("TABLE must go after CREATE");
    }
    getLex();
    TABLE_NAME();
    if (currentLexType != LEX_LEFT_PAR) {
        throw SyntaxException("left parenthesis must go after table name in CREATE request");
    }
    getLex();
    DESCRIPTION_SET();
    if (currentLexType != LEX_RIGHT_PAR) {
        throw SyntaxException("CREATE request must must end with right parenthesis");
    }
    getLex();

}

void Parser::DROP() {
    if (currentLexType != LEX_TABLE) {
        throw SyntaxException("TABLE must go after DROP");
    }
    getLex();
    TABLE_NAME();

}

void Parser::DESCRIPTION_SET() {
    DESCRIPTION();
    while (currentLexType == LEX_COMMA) {
        getLex();
        DESCRIPTION();
    }
}

void Parser::DESCRIPTION() {
    NAME();
    FIELD_TYPE();
}

void Parser::FIELD_TYPE() {

    switch (currentLexType) {
        case LEX_TEXT: {
            getLex();
            if (currentLexType != LEX_LEFT_PAR) {
                throw SyntaxException("left parenthesis must be if Field Type is Text");
            }
            getLex();
            if (currentLexType != LEX_NUMBER) {
                throw SyntaxException("Field length must be a number");
            }
            if ((int) answer.fields[answer.fields.size() - 1].getValue().length() != atol(currentLex.getValue().c_str())) {
                throw SemanticException("Field length and specified length must be equal");
            }
            getLex();
            if (currentLexType != LEX_RIGHT_PAR) {
                throw SyntaxException("right parenthesis must be if Field Type is Text");
            }
            getLex();
            answer.types.push_back("Text");
        }

            break;
        case LEX_LONG:
            getLex();
            answer.types.push_back("Long");
            break;
        default:
            throw SemanticException("wrong Field Type");
            break;
    }

}


void Parser::WHERE() {
    if (currentLexType != LEX_WHERE) {
        throw SyntaxException("WHERE must be if request is SELECT, UPDATE or DELETE");
    }
    getLex();
    if (currentLexType == LEX_ALL) {
        getLex();
        answer.whereOperationStack.push(LEX_ALL);
    } else {
        LONG_BOOL_EXPRESSION();
        if (currentLexType == LEX_NOT) {
            getLex();
            answer.whereOperationStack.push(LEX_NOT);
        }
        if (currentLexType == LEX_LIKE || currentLexType == LEX_IN) {
            WHERE_EXPRESSION();
        } else {

        }
    }
}

void Parser::WHERE_EXPRESSION() {
    switch (currentLexType) {
        case LEX_LIKE:
            getLex();
            if (currentLexType != LEX_STRING || answer.polizStack.size() != 1 || typeCheck.back() != LEX_IDENT) {
                SyntaxException("wrong WHERE LIKE alternative");
            }
            answer.filterVector.push_back(currentLex);
            getLex();
            answer.whereOperationStack.push(LEX_LIKE);
            break;
        case LEX_IN:
            getLex();
            if (currentLexType != LEX_LEFT_PAR) {
                SyntaxException("left parenthesis must go after LEX_IN");
            }
            getLex();
            CONST_SET();
            if (currentLexType != LEX_RIGHT_PAR) {
                throw SyntaxException("left parenthesis must ends with right parenthesis");
            }
            answer.whereOperationStack.push(LEX_IN);
            getLex();
            break;
        default:
            throw SyntaxException("wrong WHERE expression");
            break;
    }
}

void Parser::CONST_SET() {
    if (currentLexType == LEX_STRING || currentLexType == LEX_NUMBER) {
        answer.filterVector.push_back(currentLex);
        getLex();
        while (currentLexType == LEX_COMMA) {
            getLex();
            answer.filterVector.push_back(currentLex);
            getLex();
        }
    } else {
        SyntaxException("wrong const");
    }
}

void Parser::LONG_BOOL_EXPRESSION() {
    SUMMAND();
    while (currentLexType == LEX_PLUS || currentLexType == LEX_MINUS || currentLexType == LEX_OR) {
        OR_OP();
        SUMMAND();
    }
}

void Parser::SUMMAND() {
    MULTIPLIER();
    while (currentLexType == LEX_STAR || currentLexType == LEX_DIV || currentLexType == LEX_MOD ||
           currentLexType == LEX_AND) {
        AND_OP();
        MULTIPLIER();
    }
}

void Parser::MULTIPLIER() {
    if (currentLexType == LEX_NOT) {
        NOT_OP();
        MULTIPLIER();
    } else if (currentLexType == LEX_NUMBER) {
        NUMBER();
    } else if (currentLexType == LEX_IDENT) {
        IDENT();
    } else if (currentLexType == LEX_STRING) {
        STRING();
    } else if (currentLexType == LEX_LEFT_PAR) {
        expressionStack.push(currentLex);
        typeStack.push(currentLexType);
        getLex();
        LONG_BOOL_EXPRESSION();
        if (currentLexType == LEX_EQUAL || currentLexType == LEX_LOWER || currentLexType == LEX_GREATER ||
            currentLexType == LEX_GREATER_EQUAL || currentLexType == LEX_LOWER_EQUAL ||
            currentLexType == LEX_NOT_EQUAL) {
            COMPARE_OP();
            LONG_BOOL_EXPRESSION();
        }
        if (currentLexType != LEX_RIGHT_PAR) {
            throw SyntaxException("left parenthesis must ends with right parenthesis");
        }
        while (expressionStack.top().getType() != LEX_LEFT_PAR) {
            answer.polizStack.push_back(expressionStack.top());
            expressionStack.pop();
        }
        while (typeStack.top() != LEX_LEFT_PAR) {
            typeCheck.push_back(typeStack.top());
            typeStack.pop();
        }
        expressionStack.pop();
        typeStack.pop();
        getLex();
    } else {
        throw SyntaxException("wrong multiplier");
    }
}

void Parser::NUMBER() {
    if (currentLexType != LEX_NUMBER) {
        throw SyntaxException("wrong number");
    }
    answer.polizStack.push_back(currentLex);
    typeCheck.push_back(currentLexType);
    getLex();
}

void Parser::STRING() {
    if (currentLexType != LEX_STRING) {
        throw SyntaxException("wrong string");
    }
    answer.polizStack.push_back(currentLex);
    typeCheck.push_back(currentLexType);
    getLex();
}

void Parser::IDENT() {
    if (currentLexType != LEX_IDENT) {
        throw SyntaxException("wrong ident");
    }
    answer.polizStack.push_back(currentLex);
    typeCheck.push_back(currentLexType);
    getLex();
}

void Parser::checkTypes() {
    std::stack<TypeOfLex> result;
    while (typeCheck.size() > 0) {
        TypeOfLex current = typeCheck.front(), first, second;
        if (current == LEX_BOOL || current == LEX_NUMBER || current == LEX_STRING || current == LEX_IDENT) {
            result.push(current);
        }//Для логических операций оба операнда должны быть логического типа
        else if (current == LEX_AND || current == LEX_OR) {
            second = result.top();
            result.pop();
            first = result.top();
            result.pop();
            if (first != LEX_BOOL || second != LEX_BOOL) {
                throw SemanticException("Operands of boolean operation must be boolean");
            }
            result.push(LEX_BOOL);
            //Для операци отношения операнды должны быть совместими по типу(тип поля дополнительно проверяется на сервере)
        } else if (current == LEX_EQUAL || current == LEX_LOWER || current == LEX_GREATER ||
                   current == LEX_GREATER_EQUAL || current == LEX_LOWER_EQUAL ||
                   current == LEX_NOT_EQUAL) {
            second = result.top();
            result.pop();
            first = result.top();
            result.pop();
            if ((((first == LEX_IDENT) || (first == LEX_STRING)) && ((second == LEX_IDENT) || (second == LEX_STRING)))
                || (((first == LEX_IDENT) || (first == LEX_NUMBER)) &&
                    ((second == LEX_IDENT) || (second == LEX_NUMBER)))) {
                //OK
            } else {
                throw SemanticException("wrong operands types");
            }
            result.push(LEX_BOOL);
        } else if (current == LEX_NOT) {
            first = result.top();
            result.pop();
            if (first != LEX_BOOL) {
                throw SemanticException("if operation is NOT, operand must have boolean type");
            }
            result.push(LEX_BOOL);
        } else {
            second = result.top();
            result.pop();
            first = result.top();
            result.pop();
            if (first == LEX_BOOL || second == LEX_BOOL) {
                throw SemanticException("if operation is about long, operands mustn't have boolean type");
            }
            result.push(LEX_NUMBER);

        }
        typeCheck.pop_front();
    }
    if ((answer.whereOperationStack.size() == 0) && (result.size() > 0) && (result.top() != LEX_BOOL)) {
        throw SemanticException("if WHERE doesn't contain LIKE, IN or ALL, it must have boolean type");
    }
    if (answer.whereOperationStack.size() > 0) {
        if (result.size() > 0) {
            if ((result.top() == LEX_BOOL) &&
                (answer.whereOperationStack.top() == LEX_IN)) {
                throw SemanticException("if WHERE contains LIKE, IN or ALL, it mustn't have boolean type");
            }
        }

    }

}

Parser::Parser(std::stringstream *stream) {
    scanner = Scanner(stream);
}

Parser::Parser() {
    scanner = Scanner();
}

void Parser::OR_OP() {
    TypeOfLex topLexType;
    if (expressionStack.size() == 0) {
        topLexType = LEX_NULL;
    } else {
        topLexType = expressionStack.top().getType();
    }
    if (topLexType == LEX_STAR || topLexType == LEX_DIV || topLexType == LEX_MOD ||
        topLexType == LEX_AND || topLexType == LEX_NOT) {
        answer.polizStack.push_back(expressionStack.top());
        expressionStack.pop();
        typeCheck.push_back(typeStack.top());
        typeStack.pop();
    }
    expressionStack.push(currentLex);
    typeStack.push(currentLexType);
    getLex();
}

void Parser::AND_OP() {
    TypeOfLex topLexType;
    if (expressionStack.size() == 0) {
        topLexType = LEX_NULL;
    } else {
        topLexType = expressionStack.top().getType();
    }
    if (topLexType == LEX_NOT) {
        answer.polizStack.push_back(expressionStack.top());
        expressionStack.pop();
        typeCheck.push_back(typeStack.top());
        typeStack.pop();
    }
    expressionStack.push(currentLex);
    typeStack.push(currentLexType);
    getLex();
}

void Parser::NOT_OP() {
    expressionStack.push(currentLex);
    typeCheck.push_back(currentLexType);
    getLex();
}

void Parser::COMPARE_OP() {
    TypeOfLex topLexType;
    if (expressionStack.size() == 0) {
        topLexType = LEX_NULL;
    } else {
        topLexType = expressionStack.top().getType();
    }
    if (topLexType == LEX_STAR || topLexType == LEX_DIV || topLexType == LEX_MOD ||
        topLexType == LEX_AND || topLexType == LEX_NOT || topLexType == LEX_PLUS || topLexType == LEX_MINUS ||
        topLexType == LEX_OR) {
        answer.polizStack.push_back(expressionStack.top());
        expressionStack.pop();
        typeCheck.push_back(typeStack.top());
        typeStack.pop();
    }
    expressionStack.push(currentLex);
    typeStack.push(currentLexType);
    getLex();
}


ParserAnswer::ParserAnswer() {

}
