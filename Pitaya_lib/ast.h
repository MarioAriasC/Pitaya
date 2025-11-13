#ifndef PITAYA_AST_H
#define PITAYA_AST_H

#include <string>
#include <vector>
#include <optional>
#include "tokens.h"

#define OPT_STATEMENT_LIST std::optional<std::vector<std::optional<Statement *> > >

//
struct Statement {
    virtual ~Statement() = default;

    explicit Statement(Token token);

    const Token token;

    [[nodiscard]] std::string tokenLiteral() const;

    [[nodiscard]] virtual std::string to_string() const;
};

struct Program {
    explicit Program(const std::vector<Statement *> &statements);

    const std::vector<Statement *> statements;

    [[nodiscard]] std::string to_string() const;
};

struct StringValue : Statement {
    StringValue(const Token &token, std::string value);

    [[nodiscard]] std::string to_string() const override;

    const std::string value;
};

struct Identifier final : StringValue {
    Identifier(const Token &token, const std::string &value);
};

struct LetStatement : Statement {
    LetStatement(const Token &token, Identifier name, std::optional<Statement *> value);

    [[nodiscard]] std::string to_string() const override;

    const Identifier name;
    const std::optional<Statement *> value;
};

struct ReturnStatement : Statement {
    ReturnStatement(const Token &token, const std::optional<Statement *> &returnValue);

    [[nodiscard]] std::string to_string() const override;

    const std::optional<Statement *> returnValue;
};

struct ExpressionStatement : Statement {
    ExpressionStatement(const Token &token, const std::optional<Statement *> &expression);

    [[nodiscard]] std::string to_string() const override;

    const std::optional<Statement *> expression;
};

template<typename T>
struct LiteralExpression : Statement {
    LiteralExpression(const Token &token, T value);

    // std::string to_string() const override;
    const T value;
};

struct IntegerLiteral final : LiteralExpression<long> {
    IntegerLiteral(const Token &token, long value);

    [[nodiscard]] std::string to_string() const override;
};

struct BooleanLiteral final : LiteralExpression<bool> {
    BooleanLiteral(const Token &token, bool value);

    [[nodiscard]] std::string to_string() const override;
};

struct PrefixExpression : Statement {
    PrefixExpression(const Token &token, std::string op, std::optional<Statement *> right);

    [[nodiscard]] std::string to_string() const override;

    const std::string op;
    const std::optional<Statement *> right;
};

struct InfixExpression : Statement {
    InfixExpression(const Token &token,
                    std::optional<Statement *> left,
                    std::string op,
                    std::optional<Statement *> right);

    [[nodiscard]] std::string to_string() const override;

    const std::optional<Statement *> left;
    const std::string op;
    const std::optional<Statement *> right;
};

struct CallExpression : Statement {
    CallExpression(const Token &token,
                   std::optional<Statement *> function,
                   const OPT_STATEMENT_LIST &arguments);

    [[nodiscard]] std::string to_string() const override;

    const std::optional<Statement *> function;
    const OPT_STATEMENT_LIST arguments;
};

struct ArrayLiteral : Statement {
    ArrayLiteral(const Token &token, const OPT_STATEMENT_LIST &elements);

    [[nodiscard]] std::string to_string() const override;

    const OPT_STATEMENT_LIST elements;
};

struct IndexExpression : Statement {
    IndexExpression(const Token &token, std::optional<Statement *> left, std::optional<Statement *> index);

    [[nodiscard]] std::string to_string() const override;

    const std::optional<Statement *> left;
    const std::optional<Statement *> index;
};
#endif //PITAYA_AST_H
