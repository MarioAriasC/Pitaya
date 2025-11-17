#ifndef PITAYA_AST_H
#define PITAYA_AST_H

#include <map>
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

    bool operator<(const Statement &s) const;
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

struct StringLiteral final :StringValue {
    StringLiteral(const Token &token, const std::string &value);
};

struct LetStatement final : Statement {
    LetStatement(const Token &token, Identifier name, std::optional<Statement *> value);

    [[nodiscard]] std::string to_string() const override;

    const Identifier name;
    const std::optional<Statement *> value;
};

struct ReturnStatement final : Statement {
    ReturnStatement(const Token &token, const std::optional<Statement *> &returnValue);

    [[nodiscard]] std::string to_string() const override;

    const std::optional<Statement *> returnValue;
};

struct ExpressionStatement final : Statement {
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

struct PrefixExpression final : Statement {
    PrefixExpression(const Token &token, std::string op, std::optional<Statement *> right);

    [[nodiscard]] std::string to_string() const override;

    const std::string op;
    const std::optional<Statement *> right;
};

struct InfixExpression final : Statement {
    InfixExpression(const Token &token,
                    std::optional<Statement *> left,
                    std::string op,
                    std::optional<Statement *> right);

    [[nodiscard]] std::string to_string() const override;

    const std::optional<Statement *> left;
    const std::string op;
    const std::optional<Statement *> right;
};

struct CallExpression final : Statement {
    CallExpression(const Token &token,
                   std::optional<Statement *> function,
                   const OPT_STATEMENT_LIST &arguments);

    [[nodiscard]] std::string to_string() const override;

    const std::optional<Statement *> function;
    const OPT_STATEMENT_LIST arguments;
};

struct ArrayLiteral final : Statement {
    ArrayLiteral(const Token &token, const OPT_STATEMENT_LIST &elements);

    [[nodiscard]] std::string to_string() const override;

    const OPT_STATEMENT_LIST elements;
};

struct IndexExpression final : Statement {
    IndexExpression(const Token &token, std::optional<Statement *> left, std::optional<Statement *> index);

    [[nodiscard]] std::string to_string() const override;

    const std::optional<Statement *> left;
    const std::optional<Statement *> index;
};

struct BlockStatement final : Statement {
    BlockStatement(const Token &token, const OPT_STATEMENT_LIST &statements);

    [[nodiscard]] std::string to_string() const override;

    const OPT_STATEMENT_LIST statements;
};

struct IfExpression final : Statement {
    IfExpression(const Token &token,
                 std::optional<Statement *> condition,
                 std::optional<BlockStatement *> consequence,
                 std::optional<BlockStatement *> alternative);

    [[nodiscard]] std::string to_string() const override;

    const std::optional<Statement *> condition;
    const std::optional<BlockStatement *> consequence;
    const std::optional<BlockStatement *> alternative;
};

struct FunctionLiteral final : Statement {
    FunctionLiteral(const Token &token,
                    const std::optional<std::vector<Identifier *> > &parameters,
                    std::optional<BlockStatement *> body);

    const std::optional<std::vector<Identifier *> > parameters;
    const std::optional<BlockStatement *> body;
    std::string name;
};

struct HashLiteral final :Statement {
    HashLiteral(const Token &token, const std::map<Statement *, Statement *> &pairs);
    const std::map<Statement *, Statement *> pairs;
};
#endif //PITAYA_AST_H
