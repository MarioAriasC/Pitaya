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

    BlockStatement *parseBlockStatement();

    std::optional<std::vector<Identifier *>> parseFunctionParameters();

    // prefix parsers
    std::optional<Statement *> parseIntegerLiteral() const;

    std::optional<Statement *> parseIdentifier() const;

    std::optional<Statement *> parseBooleanLiteral() const;

    std::optional<Statement *> parsePrefixExpression();

    std::optional<Statement *> parseGroupExpression();

    std::optional<Statement *> parseArrayLiteral();

    std::optional<Statement *> parseIfExpression();

    std::optional<Statement *> parseFunctionLiteral();

    std::optional<Statement *> parseStringLiteral() const;

    std::optional<Statement *> parseHashLiteral();

    // infix parsers
    std::optional<Statement *> parseInfixExpression(std::optional<Statement *> left);

    std::optional<Statement *> parseCallExpression(std::optional<Statement *> left);

    std::optional<Statement *> parseIndexExpression(std::optional<Statement *> left);
};
#endif //PITAYA_PARSER_H
