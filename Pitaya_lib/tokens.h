#ifndef PITAYA_TOKENS_H
#define PITAYA_TOKENS_H
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

inline const char *to_string(const TokenType e) {
    switch (e) {
        case TokenType::ILLEGAL: return "ILLEGAL";
        case TokenType::EOF_: return "EOF";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::EQ: return "EQ";
        case TokenType::NOT_EQ: return "NOT_EQ";
        case TokenType::IDENT: return "IDENT";
        case TokenType::INT: return "INT";
        case TokenType::PLUS: return "PLUS";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::MINUS: return "MINUS";
        case TokenType::BANG: return "BANG";
        case TokenType::SLASH: return "SLASH";
        case TokenType::ASTERISK: return "ASTERISK";
        case TokenType::LT: return "LT";
        case TokenType::GT: return "GT";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::FUNCTION: return "FUNCTION";
        case TokenType::LET: return "LET";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::RETURN: return "RETURN";
        case TokenType::STRING: return "STRING";
        default: return "unknown";
    }
}

struct Token {
    TokenType tokenType;
    std::string literal;

    Token(TokenType tokenType, std::string literal);

    Token(TokenType tokenType, char ch);
};

TokenType lookupIdent(const std::string &literal);
#endif //PITAYA_TOKENS_H
