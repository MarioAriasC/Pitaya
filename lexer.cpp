#include <string>
#include <utility>
#include "tokens.h"
#include "lexer.h"

namespace LexerUtil {
    bool is_identifier(const char c) {
        return std::isalpha(c) || c == '_';
    }

    bool is_whitespace(const char c) {
        for (const char &whitespace: WHITESPACES) {
            if (whitespace == c) return true;
        }
        return false;
    }
}

Lexer::Lexer(std::string input) : input{std::move(input)} {
    readChar();
}

Token *Lexer::nextToken() {
    skipWhitespaces();
    Token *r = nullptr;
    switch (ch) {
        case '=':
            r = endsWithEqual(TokenType::ASSIGN, TokenType::EQ, true);
            break;
        case '!':
            r = endsWithEqual(TokenType::BANG, TokenType::NOT_EQ, false);
            break;
        case ';':
            r = token(TokenType::SEMICOLON);
            break;
        case ':':
            r = token(TokenType::COLON);
            break;
        case ',':
            r = token(TokenType::COMMA);
            break;
        case '(':
            r = token(TokenType::LPAREN);
            break;
        case ')':
            r = token(TokenType::RPAREN);
            break;
        case '{':
            r = token(TokenType::LBRACE);
            break;
        case '}':
            r = token(TokenType::RBRACE);
            break;
        case '[':
            r = token(TokenType::LBRACKET);
            break;
        case ']':
            r = token(TokenType::RBRACKET);
            break;
        case '+':
            r = token(TokenType::PLUS);
            break;
        case '-':
            r = token(TokenType::MINUS);
            break;
        case '*':
            r = token(TokenType::ASTERISK);
            break;
        case '/':
            r = token(TokenType::SLASH);
            break;
        case '<':
            r = token(TokenType::LT);
            break;
        case '>':
            r = token(TokenType::GT);
            break;
        case ZERO:
            r = new Token(TokenType::EOF_, "");
            break;
        default:
            if (LexerUtil::is_identifier(ch)) {
                const auto identifier = readIdentifier();
                return new Token(lookupIdent(identifier), identifier);
            }
            if (isdigit(ch)) {
                return new Token(TokenType::INT, readNumber());
            }
            return new Token(TokenType::ILLEGAL, ch);
    }
    readChar();
    return r;
}


void Lexer::readChar() {
    ch = peakChar();
    position = readPosition;
    readPosition++;
}

[[nodiscard]] char Lexer::peakChar() const {
    if (readPosition >= input.length()) {
        return ZERO;
    }
    return input[readPosition];
}

std::string Lexer::readNumber() {
    return readValue([](const char c) { return std::isdigit(c); });
}

std::string Lexer::readIdentifier() {
    return readValue([](const char c) { return LexerUtil::is_identifier(c); });
}

std::string Lexer::readString() {
    const auto start = position + 1;
    while (true) {
        readChar();
        if (ch == '"' || ch == ZERO) {
            break;
        }
    }
    return input.substr(start, position);
}

void Lexer::skipWhitespaces() {
    while (LexerUtil::is_whitespace(ch)) {
        readChar();
    }
}

Token *Lexer::token(const TokenType tokenType) const {
    return new Token(tokenType, ch);
}

char Lexer::peakChar() {
    if (readPosition >= input.length()) {
        return ZERO;
    }
    return input[readPosition];
}

Token *Lexer::endsWithEqual(const TokenType oneChar, const TokenType twoChars, const bool duplicateChars) {
    if (peakChar() == '=') {
        const auto currentChar = ch;
        readChar();
        if (duplicateChars) {
            return new Token(twoChars, std::string{currentChar, currentChar});
        }
        return new Token(twoChars, std::string{currentChar, ch});
    }
    return token(oneChar);
}

template<typename Fn>
std::string Lexer::readValue(Fn predicate) {
    const auto currentPosition = position;
    while (predicate(ch)) {
        readChar();
    }
    return input.substr(currentPosition, position);
}
