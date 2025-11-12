#include "parser.h"

#include <sstream>
#include <utility>

namespace ParserUtils {
    const auto INVALID = new Token(TokenType::ILLEGAL, '_');
}

Parser::Parser(Lexer lexer) : lexer{std::move(lexer)}, curToken(ParserUtils::INVALID), peekToken(ParserUtils::INVALID) {
    nextToken();
    nextToken();
}

Program *Parser::parseProgram() {
    auto statements = std::vector<Statement *>{};
    while (curToken->tokenType != TokenType::EOF_) {
        if (auto statement = parseStatement(); statement.has_value()) {
            statements.push_back(statement.value());
        }
        nextToken();
    }
    return new Program{statements};
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = lexer.nextToken();
}

std::optional<Statement *> Parser::parseStatement() {
    switch (curToken->tokenType) {
        case TokenType::LET:
            return parseLetStatement();
        case TokenType::RETURN:
            return parseReturnStatement();
        default:
            return parseExpressionStatement();
    }
}

std::optional<Statement *> Parser::parseLetStatement() {
    const auto token = curToken;
    if (!expectPeek(TokenType::IDENT)) {
        return std::nullopt;
    }
    const auto name = new Identifier(*curToken, curToken->literal);
    if (!expectPeek(TokenType::ASSIGN)) {
        return std::nullopt;
    }
    nextToken();
    const auto value = parseExpression(Precedence::LOWEST);

    if (peekTokenIs(TokenType::SEMICOLON)) {
        nextToken();
    }
    return std::optional{new LetStatement(*token, *name, value)};
}

std::optional<Statement *> Parser::parseReturnStatement() {
    const auto token = curToken;
    nextToken();
    const auto returnValue = parseExpression(Precedence::LOWEST);
    while (peekTokenIs(TokenType::SEMICOLON)) {
        nextToken();
    }
    return std::optional{new ReturnStatement(*token, returnValue)};
}

std::optional<Statement *> Parser::parseExpressionStatement() {
    const auto token = curToken;
    const auto expression = parseExpression(Precedence::LOWEST);
    if (peekTokenIs(TokenType::SEMICOLON)) {
        nextToken();
    }
    return std::optional{new ExpressionStatement(*token, expression)};
}

bool Parser::peekTokenIs(const TokenType tt) const {
    return peekToken->tokenType == tt;
}

bool Parser::expectPeek(const TokenType tt) {
    if (peekTokenIs(tt)) {
        nextToken();
        return true;
    }
    peekError(tt);
    return false;
}


void Parser::peekError(const TokenType tt) {
    std::stringstream stream;
    stream << "Expected next token to be " << to_string(tt) << ", got " << to_string(peekToken->tokenType) <<
            " instead";
    errors.push_back(stream.str());
}

bool Parser::curTokenIs(const TokenType tt) const {
    return tt == curToken->tokenType;
}

std::optional<Statement *> Parser::parseExpression(const Precedence precedence) {
    const auto prefix = prefixParser(curToken->tokenType);
    if (!prefix.has_value()) {
        noPrefixParserError(curToken->tokenType);
        return std::nullopt;
    }
    auto left = prefix.value()(this);
    while (!peekTokenIs(TokenType::SEMICOLON) && precedence < peekPrecedence()) {
        const auto infix = infixParser(peekToken->tokenType);
        if (!infix.has_value()) {
            return left;
        }
        nextToken();
        left.emplace(infix.value()(this, left).value());
    }
    return left;
}


std::optional<PREFIX_FN_TYPE > Parser::prefixParser(const TokenType tt) {
    switch (tt) {
        case TokenType::INT:
            return std::optional{static_cast<PREFIX_FN_TYPE>(&Parser::parseIntegerLiteral)};
        case TokenType::IDENT:
            return std::optional{static_cast<PREFIX_FN_TYPE>(&Parser::parseIdentifier)};
        case TokenType::TRUE:
        case TokenType::FALSE:
            return std::optional{static_cast<PREFIX_FN_TYPE>(&Parser::parseBooleanLiteral)};
        case TokenType::BANG:
        case TokenType::MINUS:
            return std::optional{static_cast<PREFIX_FN_TYPE>(&Parser::parsePrefixExpression)};
        default:
            return std::nullopt;
    }
}

std::optional<INFIX_FN_TYPE > Parser::infixParser(const TokenType tt) {
    switch (tt) {
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::SLASH:
        case TokenType::ASTERISK:
        case TokenType::EQ:
        case TokenType::NOT_EQ:
        case TokenType::LT:
        case TokenType::GT:
            return std::optional{static_cast<INFIX_FN_TYPE>(&Parser::parserInfixExpression)};
        default:
            return std::nullopt;
    }
}

void Parser::noPrefixParserError(const TokenType tt) {
    std::stringstream stream;
    stream << "no prefix parser for " << to_string(tt) << " token type";
    errors.push_back(stream.str());
}

Precedence Parser::peekPrecedence() const {
    return findPrecedence(peekToken->tokenType);
}

Precedence Parser::findPrecedence(const TokenType tt) {
    switch (tt) {
        case TokenType::EQ:
        case TokenType::NOT_EQ:
            return Precedence::EQUALS;
        case TokenType::LT:
        case TokenType::GT:
            return Precedence::LESS_GREATER;
        case TokenType::PLUS:
        case TokenType::MINUS:
            return Precedence::SUM;
        case TokenType::SLASH:
        case TokenType::ASTERISK:
            return Precedence::PRODUCT;
        case TokenType::LPAREN:
            return Precedence::CALL;
        case TokenType::LBRACKET:
            return Precedence::INDEX;
        default:
            return Precedence::LOWEST;
    }
}

Precedence Parser::currentPrecedence() const {
    return findPrecedence(curToken->tokenType);
}

std::optional<Statement *> Parser::parseIntegerLiteral() {
    const auto token = curToken;
    const long value = std::stol(token->literal);
    return std::optional{new IntegerLiteral(*token, value)};
}

std::optional<Statement *> Parser::parseIdentifier() {
    return std::optional{new Identifier(*curToken, curToken->literal)};
}

std::optional<Statement *> Parser::parseBooleanLiteral() {
    return std::optional{new BooleanLiteral(*curToken, curTokenIs(TokenType::TRUE))};
}

std::optional<Statement *> Parser::parsePrefixExpression() {
    const auto token = curToken;
    const auto op = token->literal;
    nextToken();
    const auto right = parseExpression(Precedence::PREFIX);
    return std::optional{new PrefixExpression(*token, op, right)};
}

std::optional<Statement *> Parser::parserInfixExpression(std::optional<Statement *> left) {
    const auto token = curToken;
    const auto op = token->literal;
    const auto precedence = currentPrecedence();
    nextToken();
    const auto right = parseExpression(precedence);
    return std::optional{new InfixExpression(*token, left, op, right)};
}
