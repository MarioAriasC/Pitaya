#ifndef PITAYA_PARSER_H
#define PITAYA_PARSER_H

#include "ast.h"
#include "lexer.h"
#include "boost/function.hpp"
#define PREFIX_FN_TYPE boost::function<std::optional<Statement *> (Parser *)>
#define INFIX_FN_TYPE boost::function<std::optional<Statement *> (Parser *, std::optional<Statement *>)>

enum struct Precedence {
    LOWEST, EQUALS, LESS_GREATER, SUM, PRODUCT, PREFIX, CALL, INDEX
};

struct Parser {
    explicit Parser(Lexer lexer);

    std::vector<std::string> errors;

    Program *parseProgram();

private:
    Lexer lexer;

    Token *curToken;
    Token *peekToken;

    void nextToken();

    std::optional<Statement *> parseStatement();

    std::optional<Statement *> parseLetStatement();

    std::optional<Statement *> parseReturnStatement();

    std::optional<Statement *> parseExpressionStatement();

    [[nodiscard]] bool peekTokenIs(TokenType tt) const;

    bool expectPeek(TokenType tt);

    void peekError(TokenType tt);

    [[nodiscard]] bool curTokenIs(TokenType tt) const;

    std::optional<Statement *> parseExpression(Precedence precedence);

    std::optional<PREFIX_FN_TYPE > prefixParser(TokenType tt);

    std::optional<boost::function<std::optional<Statement *>(Parser *, std::optional<Statement *>)> >
    infixParser(TokenType tt);

    void noPrefixParserError(TokenType tt);

    [[nodiscard]] Precedence peekPrecedence() const;

    static Precedence findPrecedence(TokenType tt);

    [[nodiscard]] Precedence currentPrecedence() const;

    std::optional<std::vector<std::optional<Statement *> > > parseExpressionList(TokenType end);

    // prefix parsers
    std::optional<Statement *> parseIntegerLiteral();

    std::optional<Statement *> parseIdentifier();

    std::optional<Statement *> parseBooleanLiteral();

    std::optional<Statement *> parsePrefixExpression();

    std::optional<Statement *> parseGroupExpression();

    std::optional<Statement *> parseArrayLiteral();

    // infix parsers
    std::optional<Statement *> parseInfixExpression(std::optional<Statement *> left);

    std::optional<Statement *> parseCallExpression(std::optional<Statement *> left);

    std::optional<Statement *> parseIndexExpression(std::optional<Statement *> left);
};
#endif //PITAYA_PARSER_H
