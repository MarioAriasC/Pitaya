// //
// // Created by Jorge Mario Arias Casallas on 3/11/25.
// //
//
#include "ast.h"
//
#include <sstream>
#include <utility>
// #define EQ return ASTUtil::equalsImpl(lhs, rhs);
// #define NOT_EQ return !(lhs == rhs);
//
/*namespace ASTUtil {
     template<typename T>
     bool equalsImpl(T &lhs, T &rhs) {
         if (typeid(lhs) != typeid(rhs)) return false;
         std::stringstream l;
         std::stringstream r;
         l << lhs;
         r << rhs;
         return l.str() == r.str();
     }
}*/
//
Statement::Statement(Token token) : token{std::move(token)} {
}

std::string Statement::tokenLiteral() const {
    return token.literal;
}

//
Program::Program(const std::vector<Statement *> &statements) : statements{statements} {
}

//
//
StringValue::StringValue(const Token &token, std::string value) : Statement(token), value{std::move(value)} {
}

//
//
std::ostream &operator<<(std::ostream &os, const StringValue &obj) {
    return os << obj.value;
}

//
Identifier::Identifier(const Token &token, const std::string &value) : StringValue(token, value) {
}

//
LetStatement::LetStatement(const Token &token, Identifier name, std::optional<Statement *> value) : Statement(token),
    name{name}, value{value} {
}

template<typename T>
LiteralExpression<T>::LiteralExpression(const Token &token, T value) : Statement(token), value{value} {
}

IntegerLiteral::IntegerLiteral(const Token &token, const long value) : LiteralExpression(token, value) {
}

ReturnStatement::ReturnStatement(const Token &token, const std::optional<Statement *> &returnValue) : Statement(token),
    returnValue{returnValue} {
}

ExpressionStatement::ExpressionStatement(const Token &token,
                                         const std::optional<Statement *> &expression) : Statement(token),
    expression{expression} {
}

BooleanLiteral::BooleanLiteral(const Token &token, const bool value) : LiteralExpression(token, value) {
}
