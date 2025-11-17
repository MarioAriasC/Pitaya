// //
// // Created by Jorge Mario Arias Casallas on 3/11/25.
// //
//
#include "ast.h"
//
#include <sstream>
#include <utility>
#include <boost/algorithm/string/join.hpp>

namespace ASTUtil {
    std::string to_string(std::optional<Statement *> op) {
        return op.transform([](const Statement *st) {
            return st->to_string();
        }).value_or("");
    }

    std::string join_to_string(OPT_STATEMENT_LIST l, const std::string &separator) {
        return l.transform([separator](const std::vector<std::optional<Statement *> > &ll) {
            std::vector<std::string> sts{};
            sts.reserve(ll.size());
            for (const auto statement: ll) {
                sts.push_back(to_string(statement));
            }
            return boost::algorithm::join(sts, separator);
        }).value_or("");
    }
}

//
Statement::Statement(Token token) : token{std::move(token)} {
}

std::string Statement::tokenLiteral() const {
    return token.literal;
}

std::string Statement::to_string() const {
    return "Statement";
}

bool Statement::operator<(const Statement &s) const {
    return to_string() < s.to_string();
}

Program::Program(const std::vector<Statement *> &statements) : statements{statements} {
}

std::string Program::to_string() const {
    std::stringstream ss;
    for (const auto statement: statements) {
        ss << statement->to_string();
    }
    return ss.str();
}

StringValue::StringValue(const Token &token, std::string value) : Statement(token), value{std::move(value)} {
}

std::string StringValue::to_string() const {
    return value;
}

Identifier::Identifier(const Token &token, const std::string &value) : StringValue(token, value) {
}

LetStatement::LetStatement(const Token &token, Identifier name, std::optional<Statement *> value) : Statement(token),
    name{std::move(name)}, value{value} {
}

std::string LetStatement::to_string() const {
    std::stringstream ss;
    ss << tokenLiteral() << " " << name.to_string() << " = " << ASTUtil::to_string(value) << ";";
    return ss.str();
}

template<typename T>
LiteralExpression<T>::LiteralExpression(const Token &token, T value) : Statement(token), value{value} {
}

// template<typename T>
// std::string LiteralExpression<long>::to_string() const {
// return Statement::to_string();
// }

IntegerLiteral::IntegerLiteral(const Token &token, const long value) : LiteralExpression(token, value) {
}

std::string IntegerLiteral::to_string() const {
    return std::to_string(value);
}

ReturnStatement::ReturnStatement(const Token &token, const std::optional<Statement *> &returnValue) : Statement(token),
    returnValue{returnValue} {
}

std::string ReturnStatement::to_string() const {
    std::stringstream ss;
    ss << tokenLiteral() << " " << ASTUtil::to_string(returnValue) << ";";
    return ss.str();
}

ExpressionStatement::ExpressionStatement(const Token &token,
                                         const std::optional<Statement *> &expression) : Statement(token),
    expression{expression} {
}

std::string ExpressionStatement::to_string() const {
    return ASTUtil::to_string(expression);
}

BooleanLiteral::BooleanLiteral(const Token &token, const bool value) : LiteralExpression(token, value) {
}

std::string BooleanLiteral::to_string() const {
    return value ? "true" : "false";
}

PrefixExpression::PrefixExpression(const Token &token,
                                   std::string op,
                                   const std::optional<Statement *> right) : Statement(token),
                                                                             op{std::move(op)},
                                                                             right{right} {
}

std::string PrefixExpression::to_string() const {
    std::stringstream ss;
    ss << "(" << op << ASTUtil::to_string(right) << ")";
    return ss.str();
}

InfixExpression::InfixExpression(const Token &token,
                                 const std::optional<Statement *> left,
                                 std::string op,
                                 const std::optional<Statement *> right) : Statement(token),
                                                                           left{left},
                                                                           op{std::move(op)},
                                                                           right{right} {
}

std::string InfixExpression::to_string() const {
    std::stringstream ss;
    ss << "(" << ASTUtil::to_string(left) << " " << op << " " << ASTUtil::to_string(right) << ")";
    return ss.str();
}

CallExpression::CallExpression(const Token &token,
                               const std::optional<Statement *> function,
                               const OPT_STATEMENT_LIST &
                               arguments) : Statement(token),
                                            function{function},
                                            arguments{arguments} {
}

std::string CallExpression::to_string() const {
    std::stringstream ss;
    ss << ASTUtil::to_string(function) << "(" << ASTUtil::join_to_string(arguments, ", ") << ")";
    return ss.str();
}

ArrayLiteral::ArrayLiteral(const Token &token,
                           const OPT_STATEMENT_LIST &elements) : Statement(token),
                                                                 elements{elements} {
}

std::string ArrayLiteral::to_string() const {
    std::stringstream ss;
    ss << "[" << ASTUtil::join_to_string(elements, ", ") << "]";
    return ss.str();
}

IndexExpression::IndexExpression(const Token &token,
                                 const std::optional<Statement *> left,
                                 const std::optional<Statement *> index) : Statement(token), left{left}, index{index} {
}

std::string IndexExpression::to_string() const {
    std::stringstream ss;
    ss << "(" << ASTUtil::to_string(left) << "[" << ASTUtil::to_string(index) << "])";
    return ss.str();
}

BlockStatement::BlockStatement(const Token &token, const OPT_STATEMENT_LIST &statements) : Statement(token),
    statements{statements} {
}

std::string BlockStatement::to_string() const {
    return ASTUtil::join_to_string(statements, "");
}

IfExpression::IfExpression(const Token &token,
                           const std::optional<Statement *> condition,
                           const std::optional<BlockStatement *> consequence,
                           const std::optional<BlockStatement *> alternative) : Statement(token), condition{condition},
    consequence{consequence}, alternative{alternative} {
}

std::string IfExpression::to_string() const {
    std::stringstream ss;
    ss << "if " << ASTUtil::to_string(condition) << " " << ASTUtil::to_string(consequence);
    if (alternative.has_value()) {
        ss << " else " << ASTUtil::to_string(alternative);
    }
    return ss.str();
}

FunctionLiteral::FunctionLiteral(const Token &token,
                                 const std::optional<std::vector<Identifier *> > &parameters,
                                 const std::optional<BlockStatement *> body) : Statement(token),
                                                                               parameters{parameters}, body{body},
                                                                               name{""} {
}

StringLiteral::StringLiteral(const Token &token, const std::string &value) : StringValue(token, value) {
}

HashLiteral::HashLiteral(const Token &token, const std::map<Statement *, Statement *> &pairs) : Statement(token),
    pairs{pairs} {
}
