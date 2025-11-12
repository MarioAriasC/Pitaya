//
// Created by Jorge Mario Arias Casallas on 2/11/25.
//

#ifndef PITAYA_LEXER_H
#define PITAYA_LEXER_H
#include "tokens.h"
static constexpr char ZERO = 0;
static constexpr auto WHITESPACES = {' ', '\t', '\r', '\n'};

struct Lexer {
    explicit Lexer(std::string input);

    Token *nextToken();

private:
    const std::string input;
    int position = 0;
    int readPosition = 0;
    char ch = ZERO;

    void readChar();

    [[nodiscard]] char peakChar() const;

    template<typename Fn>
    std::string readValue(Fn predicate);

    std::string readNumber();

    std::string readIdentifier();

    std::string readString();

    void skipWhitespaces();

    [[nodiscard]] Token *token(TokenType tokenType) const;

    char peakChar();

    Token *endsWithEqual(TokenType oneChar, TokenType twoChars, bool duplicateChars);
};


#endif //PITAYA_LEXER_H
