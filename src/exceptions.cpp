
#include "exceptions.h"

SQLException::SQLException(const std::string &str) {
    message = str;
    mainMessage = "";
}

std::string SQLException::getMessage() const {
    return mainMessage + message;
}

NetException::NetException(const std::string &str) : SQLException(str) {
    mainMessage = "Net Error: ";
}

ReposException::ReposException(const std::string &str) : SQLException(str) {
    mainMessage = "Error in Table Repository: ";
}

TableCreatingException::TableCreatingException(const std::string &str) : SQLException(str) {
    mainMessage = "Error while manipulating tables: ";
}

IOException::IOException(const std::string &str) : SQLException(str) {
    mainMessage = "File IO error: ";
}

TableEditingException::TableEditingException(const std::string &str) : SQLException(str) {
    mainMessage = "Table editing error: ";
}

LexException::LexException(const std::string &str) : SQLException(str) {
    mainMessage = "LexError: ";
}

SyntaxException::SyntaxException(const std::string &str) : SQLException(str) {
    mainMessage = "SyntaxError: ";
}

SemanticException::SemanticException(const std::string &str) : SQLException(str) {
    mainMessage = "SemanticError: ";
}
