#include <string>
#include <utility>

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

TokenType lookupIdent(const std::string &literal) {
    if (literal == "fn") return TokenType::FUNCTION;
    if (literal == "let") return TokenType::LET;
    if (literal == "true") return TokenType::TRUE;
    if (literal == "false") return TokenType::FALSE;
    if (literal == "if") return TokenType::IF;
    if (literal == "else") return TokenType::ELSE;
    if (literal == "return") return TokenType::RETURN;
    return TokenType::IDENT;
}

struct Token {
    Token(const TokenType tokenType, std::string literal) : tokenType{tokenType}, literal{std::move(literal)} {
    }

    Token(const TokenType tokenType, const char ch) : tokenType{tokenType}, literal{ch} {
    }

    TokenType tokenType;
    std::string literal;
};
