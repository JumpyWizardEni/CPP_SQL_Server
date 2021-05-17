#ifndef SQL_P3_EXCEPTIONS_H
#define SQL_P3_EXCEPTIONS_H

#include <string>

class SQLException {
protected:
    std::string mainMessage;
    std::string message;
public:
    explicit SQLException(const std::string &str);

    std::string getMessage() const;
};

class NetException : public SQLException {
public:
    explicit NetException(const std::string &str);
};

class ReposException : public SQLException {
public:
    explicit ReposException(const std::string &str);
};

class TableCreatingException : public SQLException {
public:
    explicit TableCreatingException(const std::string &str);
};

class TableEditingException : public SQLException {
public:
    explicit TableEditingException(const std::string &str);
};

class IOException : public SQLException {
public:
    explicit IOException(const std::string &str);
};

class LexException : public SQLException {
public:
    explicit LexException(const std::string &str);
};

class SyntaxException : public SQLException {
public:
    explicit SyntaxException(const std::string &str);
};

class SemanticException : public SQLException {
public:
    explicit SemanticException(const std::string &str);
};
#endif //SQL_P3_EXCEPTIONS_H
