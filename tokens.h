#ifndef PITAYA_PARSER_H
#define PITAYA_PARSER_H
#include <string>

enum struct TokenType {
    ILLEGAL,
    EOF_,
    ASSIGN,
    EQ,
    NOT_EQ,
    IDENT,
    INT,
    PLUS,
    COMMA,
    SEMICOLON,
    COLON,
    MINUS,
    BANG,
    SLASH,
    ASTERISK,
    LT,
    GT,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    FUNCTION,
    LET,
    TRUE,
    FALSE,
    IF,
    ELSE,
    RETURN,
    STRING,
};

struct Token {
    TokenType tokenType;
    std::string literal;

    Token(TokenType tokenType, std::string literal);

    Token(TokenType tokenType, char ch);
};

TokenType lookupIdent(const std::string &literal);
#endif //PITAYA_PARSER_H
