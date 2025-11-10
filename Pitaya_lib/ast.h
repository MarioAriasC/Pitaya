#ifndef PITAYA_AST_H
#define PITAYA_AST_H
// #include <ostream>
// #include <string>
#include <vector>
//
#include <optional>

#include "tokens.h"

//
struct Statement {
    virtual ~Statement() = default;

    explicit Statement(Token token);

    const Token token;

    [[nodiscard]] std::string tokenLiteral() const;
};

struct Program {
    explicit Program(const std::vector<Statement *> &statements);

    const std::vector<Statement *> statements;
};

struct StringValue : Statement {
    StringValue(const Token &token, std::string value);

    friend std::ostream &operator<<(std::ostream &os, const StringValue &obj);

    const std::string value;
};

//
struct Identifier final : StringValue {
    Identifier(const Token &token, const std::string &value);
};

//
struct LetStatement : Statement {
    LetStatement(const Token &token, Identifier name, std::optional<Statement *> value);

    const Identifier name;
    const std::optional<Statement *> value;
};

struct ReturnStatement : Statement {
    ReturnStatement(const Token &token, const std::optional<Statement *> &returnValue);

    const std::optional<Statement *> returnValue;
};

struct ExpressionStatement : Statement {
    ExpressionStatement(const Token &token, const std::optional<Statement *> &expression);

    const std::optional<Statement *> expression;
};

template<typename T>
struct LiteralExpression : Statement {
    LiteralExpression(const Token &token, T value);

    const T value;
};


struct IntegerLiteral final : LiteralExpression<int> {
    IntegerLiteral(const Token &token, int value);
};
#endif //PITAYA_AST_H
