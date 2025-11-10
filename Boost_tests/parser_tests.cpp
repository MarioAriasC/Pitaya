#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/any.hpp>
#include <utility>

#include "parser.h"


void checkParserErrors(const Parser &parser) {
    if (const auto errors = parser.errors; !errors.empty()) {
        BOOST_FAIL("parser has " + std::to_string(errors.size()) + " errors: ");
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

void testLongLiteral(std::optional<Statement *> expression, const long l) {
    if (expression.has_value()) {
        const auto exp = static_cast<IntegerLiteral *>(expression.value());
        BOOST_REQUIRE_EQUAL(l, exp->value);
        BOOST_REQUIRE_EQUAL(std::to_string(l), exp->tokenLiteral());
    } else {
        BOOST_FAIL("none value");
    }

}

template<typename T>
void testLiteralExpression(std::optional<Statement *> value, T expectedValue) {
    if (std::is_integral_v<T>) {
        testLongLiteral(std::move(value), expectedValue);
        // return;
    }
}


BOOST_AUTO_TEST_SUITE(Parser_suite)
    BOOST_AUTO_TEST_CASE(testLetStatements) {
        auto tests = {
            std::tuple{"let x = 5;", "x", 5}
        };
        for (auto &[input, expectedIdentifier, expectedValue]: tests) {
            auto program = createProgram(input);
            countStatement(1, *program);
            auto statement = program->statements[0];
            BOOST_REQUIRE_EQUAL("let", statement->tokenLiteral());
            const auto let_statement = static_cast<LetStatement *>(statement);
            BOOST_REQUIRE_EQUAL(expectedIdentifier, let_statement->name.value);
            BOOST_REQUIRE_EQUAL(expectedIdentifier, let_statement->name.tokenLiteral());
            const auto value = let_statement->value;
            testLiteralExpression(value, expectedValue);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
