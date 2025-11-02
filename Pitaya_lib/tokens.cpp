#include <string>
#include <utility>
#include "tokens.h"


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


Token::Token(const TokenType tokenType, std::string literal) : tokenType{tokenType}, literal{std::move(literal)} {
}

Token::Token(const TokenType tokenType, const char ch) : tokenType{tokenType}, literal{ch} {
}
