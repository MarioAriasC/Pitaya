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
        case TokenType::LPAREN:
            return std::optional{static_cast<PREFIX_FN_TYPE>(&Parser::parseGroupExpression)};
        case TokenType::LBRACKET:
            return std::optional{static_cast<PREFIX_FN_TYPE>(&Parser::parseArrayLiteral)};
        case TokenType::IF:
            return std::optional{static_cast<PREFIX_FN_TYPE>(&Parser::parseIfExpression)};
        case TokenType::FUNCTION:
            return std::optional{static_cast<PREFIX_FN_TYPE>(&Parser::parseFunctionLiteral)};
        case TokenType::STRING:
            return std::optional{static_cast<PREFIX_FN_TYPE>(&Parser::parseStringLiteral)};
        case TokenType::LBRACE:
            return std::optional{static_cast<PREFIX_FN_TYPE>(&Parser::parseHashLiteral)};
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
            return std::optional{static_cast<INFIX_FN_TYPE>(&Parser::parseInfixExpression)};
        case TokenType::LPAREN:
            return std::optional{static_cast<INFIX_FN_TYPE>(&Parser::parseCallExpression)};
        case TokenType::LBRACKET:
            return std::optional{static_cast<INFIX_FN_TYPE>(&Parser::parseIndexExpression)};
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

std::optional<std::vector<std::optional<Statement *> > > Parser::parseExpressionList(const TokenType end) {
    auto arguments = std::vector<std::optional<Statement *> >{};
    if (peekTokenIs(end)) {
        nextToken();
        return std::optional{arguments};
    }
    nextToken();
    arguments.push_back(parseExpression(Precedence::LOWEST));
    while (peekTokenIs(TokenType::COMMA)) {
        nextToken();
        nextToken();
        arguments.push_back(parseExpression(Precedence::LOWEST));
    }
    if (!expectPeek(end)) {
        return std::nullopt;
    }
    return std::optional{arguments};
}

BlockStatement *Parser::parseBlockStatement() {
    const auto token = curToken;
    auto statements = std::vector<std::optional<Statement *> >{};
    nextToken();
    while (!curTokenIs(TokenType::RBRACE) && !curTokenIs(TokenType::EOF_)) {
        if (const auto statement = parseStatement(); statement.has_value()) {
            statements.push_back(statement);
        }
        nextToken();
    }
    return std::move(new BlockStatement{*token, std::optional{statements}});
}

std::optional<std::vector<Identifier *> > Parser::parseFunctionParameters() {
    auto parameters = std::vector<Identifier *>{};
    if (peekTokenIs(TokenType::RPAREN)) {
        nextToken();
        return std::optional{parameters};
    }
    nextToken();
    const auto token = curToken;
    parameters.push_back(new Identifier(*token, token->literal));
    while (peekTokenIs(TokenType::COMMA)) {
        nextToken();
        nextToken();
        const auto inner_token = curToken;
        parameters.push_back(new Identifier(*inner_token, inner_token->literal));
    }
    if (!expectPeek(TokenType::RPAREN)) {
        return std::nullopt;
    }
    return std::optional{parameters};
}

std::optional<Statement *> Parser::parseIntegerLiteral() const {
    const auto token = curToken;
    const long value = std::stol(token->literal);
    return std::optional{new IntegerLiteral(*token, value)};
}

std::optional<Statement *> Parser::parseIdentifier() const {
    return std::optional{new Identifier(*curToken, curToken->literal)};
}

std::optional<Statement *> Parser::parseBooleanLiteral() const {
    return std::optional{new BooleanLiteral(*curToken, curTokenIs(TokenType::TRUE))};
}

std::optional<Statement *> Parser::parsePrefixExpression() {
    const auto token = curToken;
    const auto op = token->literal;
    nextToken();
    const auto right = parseExpression(Precedence::PREFIX);
    return std::optional{new PrefixExpression(*token, op, right)};
}

std::optional<Statement *> Parser::parseGroupExpression() {
    nextToken();
    const auto exp = parseExpression(Precedence::LOWEST);
    if (!expectPeek(TokenType::RPAREN)) {
        return std::nullopt;
    }
    return exp;
}

std::optional<Statement *> Parser::parseArrayLiteral() {
    const auto token = curToken;
    return std::optional{new ArrayLiteral{*token, parseExpressionList(TokenType::RBRACKET)}};
}

std::optional<Statement *> Parser::parseIfExpression() {
    const auto token = curToken;
    if (!expectPeek(TokenType::LPAREN)) {
        return std::nullopt;
    }
    nextToken();
    const auto condition = parseExpression(Precedence::LOWEST);
    if (!expectPeek(TokenType::RPAREN)) {
        return std::nullopt;
    }
    if (!expectPeek(TokenType::LBRACE)) {
        return std::nullopt;
    }
    const auto consequence = parseBlockStatement();
    std::optional<BlockStatement *> alternative = std::nullopt;
    if (peekTokenIs(TokenType::ELSE)) {
        nextToken();
        if (!expectPeek(TokenType::LBRACE)) {
            return std::nullopt;
        }
        alternative.emplace(parseBlockStatement());
    }
    return std::optional{new IfExpression{*token, condition, consequence, alternative}};
}

std::optional<Statement *> Parser::parseFunctionLiteral() {
    const auto token = curToken;
    if (!expectPeek(TokenType::LPAREN)) {
        return std::nullopt;
    }
    const auto parameters = parseFunctionParameters();
    if (!expectPeek(TokenType::LBRACE)) {
        return std::nullopt;
    }
    const auto body = parseBlockStatement();
    return std::optional{new FunctionLiteral(*token, parameters, body)};
}

std::optional<Statement *> Parser::parseStringLiteral() const {
    return std::optional{new StringLiteral(*curToken, curToken->literal)};
}

std::optional<Statement *> Parser::parseHashLiteral() {
    const auto token = curToken;
    auto pairs = std::map<Statement *, Statement *>();
    while (!peekTokenIs(TokenType::RBRACE)) {
        nextToken();
        const auto key = parseExpression(Precedence::LOWEST);
        if (!expectPeek(TokenType::COLON)) {
            return std::nullopt;
        }
        nextToken();
        const auto value = parseExpression(Precedence::LOWEST);
        if (key.has_value() && value.has_value()) {
            pairs[key.value()] = value.value();
        }
        if (!peekTokenIs(TokenType::RBRACE) && !expectPeek(TokenType::COMMA)) {
            return std::nullopt;
        }
    }
    if (!expectPeek(TokenType::RBRACE)) {
        return std::nullopt;
    }
    return std::optional{new HashLiteral{*token, pairs}};
}

std::optional<Statement *> Parser::parseInfixExpression(const std::optional<Statement *> left) {
    const auto token = curToken;
    const auto op = token->literal;
    const auto precedence = currentPrecedence();
    nextToken();
    const auto right = parseExpression(precedence);
    return std::optional{new InfixExpression(*token, left, op, right)};
}

std::optional<Statement *> Parser::parseCallExpression(const std::optional<Statement *> left) {
    const auto token = curToken;
    const auto arguments = parseExpressionList(TokenType::RPAREN);
    return std::optional{new CallExpression(*token, left, arguments)};
}

std::optional<Statement *> Parser::parseIndexExpression(const std::optional<Statement *> left) {
    const auto token = curToken;
    nextToken();
    const auto index = parseExpression(Precedence::LOWEST);
    if (!expectPeek(TokenType::RBRACKET)) {
        return std::nullopt;
    }
    return std::optional{new IndexExpression{*token, left, index}};
}
