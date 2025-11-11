#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/any.hpp>
#include <boost/algorithm/string/join.hpp>
#include <utility>
#include <variant>

#include "parser.h"


void checkParserErrors(const Parser &parser) {
    if (const auto errors = parser.errors; !errors.empty()) {
        BOOST_FAIL("parser has " + std::to_string(errors.size()) + " errors: " + boost::algorithm::join(errors, ", "));
    }
}

void countStatement(const int i, const Program &program) {
    BOOST_REQUIRE_EQUAL(i, program.statements.size());
}


Program *createProgram(std::string input) {
    const auto lexer = new Lexer(std::move(input));
    const auto parser = new Parser(*lexer);
    const auto program = parser->parseProgram();
    checkParserErrors(*parser);
    return program;
}

template<typename Fn>
void process(std::optional<Statement *> o, Fn fn) {
    if (o.has_value()) {
        fn(o.value());
    } else {
        BOOST_FAIL("none value");
    }
}

void testLongLiteral(const std::optional<Statement *> expression, const long l) {
    process(expression, [l](Statement *st) {
        const auto exp = static_cast<IntegerLiteral *>(st);
        BOOST_REQUIRE_EQUAL(l, exp->value);
        BOOST_REQUIRE_EQUAL(std::to_string(l), exp->tokenLiteral());
    });
}

void testBooleanLiteral(const std::optional<Statement *> expression, const bool b) {
    process(expression, [b](Statement *st) {
        const auto exp = static_cast<BooleanLiteral *>(st);
        BOOST_REQUIRE_EQUAL(b, exp->value);
        const auto b_string = b ? "true" : "false";
        BOOST_REQUIRE_EQUAL(b_string, exp->tokenLiteral());
    });
}

void testIdentifier(const std::optional<Statement *> expression, const std::string s) {
    process(expression, [s](Statement *st) {
        const auto exp = static_cast<Identifier *>(st);
        BOOST_REQUIRE_EQUAL(s, exp->value);
        BOOST_REQUIRE_EQUAL(s, exp->tokenLiteral());
    });
}

#define VARIANT_TYPE std::variant<long, bool, std::string>

void testLiteralExpression(std::optional<Statement *> value, const VARIANT_TYPE &expectedValue) {
    if (std::holds_alternative<long>(expectedValue)) {
        testLongLiteral(std::move(value), std::get<long>(expectedValue));
    } else if (std::holds_alternative<bool>(expectedValue)) {
        testBooleanLiteral(std::move(value), std::get<bool>(expectedValue));
    } else if (std::holds_alternative<std::string>(expectedValue)) {
        testIdentifier(std::move(value), std::get<std::string>(expectedValue));
    } else {
        BOOST_FAIL("not implemented");
    }
}


BOOST_AUTO_TEST_SUITE(Parser_suite)
    BOOST_AUTO_TEST_CASE(testLetStatements) {
        std::initializer_list<std::tuple<std::string, std::string, VARIANT_TYPE > > tests = {
            std::tuple{"let x = 5;", "x", 5},
            std::tuple{"let y = true;", "y", true},
            std::tuple{"let foobar = y;", "foobar", "y"}
        };
        for (auto &[input, expectedIdentifier, expectedValue]: tests) {
            const auto program = createProgram(input);
            countStatement(1, *program);
            const auto statement = program->statements[0];
            BOOST_REQUIRE_EQUAL("let", statement->tokenLiteral());
            const auto let_statement = static_cast<LetStatement *>(statement);
            BOOST_REQUIRE_EQUAL(expectedIdentifier, let_statement->name.value);
            BOOST_REQUIRE_EQUAL(expectedIdentifier, let_statement->name.tokenLiteral());
            const auto value = let_statement->value;
            testLiteralExpression(value, expectedValue);
        }
    }

    BOOST_AUTO_TEST_CASE(testReturnStatement) {
        std::initializer_list<std::tuple<std::string, VARIANT_TYPE > > tests = {
            std::tuple{"return 5;", 5},
            std::tuple{"return true;", true},
            std::tuple{"return foobar;", "foobar"}
        };
        for (auto &[input, expectedValue]: tests) {
            const auto program = createProgram(input);
            countStatement(1, *program);
            const auto return_statement = static_cast<ReturnStatement *>(program->statements[0]);
            BOOST_REQUIRE_EQUAL("return", return_statement->tokenLiteral());
            testLiteralExpression(return_statement->returnValue, expectedValue);
        }
    }


BOOST_AUTO_TEST_SUITE_END()
