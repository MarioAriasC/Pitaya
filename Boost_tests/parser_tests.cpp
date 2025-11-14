#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/any.hpp>
#include <boost/algorithm/string/join.hpp>
#include <utility>
#include <variant>
#include <iostream>

#include "parser.h"


std::string to_string_b(const bool b) {
    return b ? "true" : "false";
}

void checkParserErrors(const Parser &parser) {
    if (const auto errors = parser.errors; !errors.empty()) {
        BOOST_FAIL("parser has " + std::to_string(errors.size()) + " errors: " + boost::algorithm::join(errors, ", "));
    }
}

void countStatements(const int i, const Program &program) {
    BOOST_REQUIRE_EQUAL(i, program.statements.size());
}


Program *createProgram(std::string input) {
    const auto lexer = new Lexer(std::move(input));
    const auto parser = new Parser(*lexer);
    const auto program = parser->parseProgram();
    checkParserErrors(*parser);
    return program;
}

template<typename T, typename Fn>
void processT(const std::optional<T *> o, const Fn &fn) {
    if (o.has_value()) {
        fn(o.value());
    } else {
        BOOST_FAIL("none value");
    }
}

template<typename Fn>
void process(std::optional<Statement *> o, const Fn &fn) {
    processT(o, fn);
}


void testLongLiteral(const std::optional<Statement *> expression, const long l) {
    process(expression, [l](Statement *st) {
        const auto exp = dynamic_cast<IntegerLiteral *>(st);
        BOOST_REQUIRE_EQUAL(l, exp->value);
        BOOST_REQUIRE_EQUAL(std::to_string(l), exp->tokenLiteral());
    });
}

void testBooleanLiteral(const std::optional<Statement *> expression, const bool b) {
    process(expression, [b](Statement *st) {
        const auto exp = dynamic_cast<BooleanLiteral *>(st);
        BOOST_REQUIRE_EQUAL(b, exp->value);
        BOOST_REQUIRE_EQUAL(to_string_b(b), exp->tokenLiteral());
    });
}

void testIdentifier(const std::optional<Statement *> expression, const std::string &s) {
    process(expression, [s](Statement *st) {
        const auto exp = dynamic_cast<Identifier *>(st);
        BOOST_REQUIRE_EQUAL(s, exp->value);
        BOOST_REQUIRE_EQUAL(s, exp->tokenLiteral());
    });
}

#define VARIANT_TYPE std::variant<long, bool, std::string>

void testLiteralExpression(const std::optional<Statement *> value, const VARIANT_TYPE &expected_value) {
    if (std::holds_alternative<long>(expected_value)) {
        testLongLiteral(value, std::get<long>(expected_value));
    } else if (std::holds_alternative<bool>(expected_value)) {
        testBooleanLiteral(value, std::get<bool>(expected_value));
    } else if (std::holds_alternative<std::string>(expected_value)) {
        testIdentifier(value, std::get<std::string>(expected_value));
    } else {
        BOOST_FAIL("not implemented");
    }
}

void testInfixExpression(const std::optional<Statement *> expression,
                         const VARIANT_TYPE &left_value,
                         const std::string &op,
                         const VARIANT_TYPE &right_value) {
    process(expression, [&](Statement *st) {
        const auto exp = dynamic_cast<InfixExpression *>(st);
        testLiteralExpression(exp->left, left_value);
        BOOST_REQUIRE_EQUAL(op, exp->op);
        testLiteralExpression(exp->right, right_value);
    });
}

void testBlockStatement(const std::optional<BlockStatement *> blk, const std::string &identifier_name) {
    processT(blk, [&](const BlockStatement *consequenceBlk) {
        if (const auto statements = consequenceBlk->statements; statements.has_value()) {
            const auto &sts = statements.value();
            BOOST_REQUIRE_EQUAL(1, sts.size());
            process(sts[0], [&](Statement *cons_st) {
                const auto consequence = dynamic_cast<ExpressionStatement *>(cons_st);
                testIdentifier(consequence->expression, identifier_name);
            });
        } else {
            BOOST_FAIL("statements is empty");
        }
    });
}


BOOST_AUTO_TEST_SUITE(Parser_suite)
    BOOST_AUTO_TEST_CASE(testLetStatements) {
        std::initializer_list<std::tuple<std::string, std::string, VARIANT_TYPE > > tests = {
            std::tuple{"let x = 5;", "x", 5},
            std::tuple{"let y = true;", "y", true},
            std::tuple{"let foobar = y;", "foobar", "y"}
        };
        for (auto &[input, expected_identifier, expected_value]: tests) {
            const auto program = createProgram(input);
            countStatements(1, *program);
            const auto statement = program->statements[0];
            BOOST_REQUIRE_EQUAL("let", statement->tokenLiteral());
            const auto let_statement = dynamic_cast<LetStatement *>(statement);
            BOOST_REQUIRE_EQUAL(expected_identifier, let_statement->name.value);
            BOOST_REQUIRE_EQUAL(expected_identifier, let_statement->name.tokenLiteral());
            const auto value = let_statement->value;
            testLiteralExpression(value, expected_value);
        }
    }

    BOOST_AUTO_TEST_CASE(testReturnStatement) {
        std::initializer_list<std::tuple<std::string, VARIANT_TYPE > > tests = {
            std::tuple{"return 5;", 5},
            std::tuple{"return true;", true},
            std::tuple{"return foobar;", "foobar"}
        };
        for (auto &[input, expected_value]: tests) {
            const auto program = createProgram(input);
            countStatements(1, *program);
            const auto return_statement = dynamic_cast<ReturnStatement *>(program->statements[0]);
            BOOST_REQUIRE_EQUAL("return", return_statement->tokenLiteral());
            testLiteralExpression(return_statement->returnValue, expected_value);
        }
    }

    BOOST_AUTO_TEST_CASE(testIdentifierLiteral) {
        const auto input = "foobar";
        const auto program = createProgram(input);
        countStatements(1, *program);
        const auto expression_statement = dynamic_cast<ExpressionStatement *>(program->statements[0]);
        process(expression_statement->expression, [](Statement *st) {
            const auto identifier = dynamic_cast<Identifier *>(st);
            BOOST_REQUIRE_EQUAL("foobar", identifier->value);
            BOOST_REQUIRE_EQUAL("foobar", identifier->tokenLiteral());
        });
    }

    BOOST_AUTO_TEST_CASE(testIntegerLiteral) {
        const auto input = "5";
        const auto program = createProgram(input);
        countStatements(1, *program);
        const auto expression_statement = dynamic_cast<ExpressionStatement *>(program->statements[0]);
        testLongLiteral(expression_statement->expression, 5);
    }

    BOOST_AUTO_TEST_CASE(testPrefixExpressions) {
        std::initializer_list<std::tuple<std::string, std::string, VARIANT_TYPE > > tests = {
            std::tuple{"!5;", "!", 5},
            std::tuple{"-15;", "-", 15},
            std::tuple{"!true;", "!", true},
            std::tuple{"!false;", "!", false},
        };

        for (auto &[input, op, expected_value]: tests) {
            const auto program = createProgram(input);
            countStatements(1, *program);
            const auto expression_statement = dynamic_cast<ExpressionStatement *>(program->statements[0]);
            process(expression_statement->expression, [&](Statement *exp) {
                const auto expression = dynamic_cast<PrefixExpression *>(exp);
                BOOST_REQUIRE_EQUAL(op, expression->op);
                testLiteralExpression(expression->right, expected_value);
            });
        }
    }

    BOOST_AUTO_TEST_CASE(testInfixExpressions) {
        std::initializer_list<std::tuple<std::string, VARIANT_TYPE, std::string, VARIANT_TYPE > > tests = {
            std::tuple{"5 + 5;", 5, "+", 5},
            std::tuple{"5 - 5;", 5, "-", 5},
            std::tuple{"5 * 5;", 5, "*", 5},
            std::tuple{"5 / 5;", 5, "/", 5},
            std::tuple{"5 > 5;", 5, ">", 5},
            std::tuple{"5 < 5;", 5, "<", 5},
            std::tuple{"5 == 5;", 5, "==", 5},
            std::tuple{"5 != 5;", 5, "!=", 5},
            std::tuple{"true == true", true, "==", true},
            std::tuple{"true != false", true, "!=", false},
            std::tuple{"false == false", false, "==", false},
        };
        for (auto &[input, left_value, op, right_value]: tests) {
            const auto program = createProgram(input);
            countStatements(1, *program);
            const auto expression_statement = dynamic_cast<ExpressionStatement *>(program->statements[0]);
            process(expression_statement->expression, [&](Statement *exp) {
                testInfixExpression(exp, left_value, op, right_value);
            });
        }
    }

    BOOST_AUTO_TEST_CASE(testOperatorPrecedence) {
        const auto tests = {
            std::tuple{"-a * b", "((-a) * b)"},
            std::tuple{"!-a", "(!(-a))"},
            std::tuple{"a + b + c", "((a + b) + c)"},
            std::tuple{"a + b - c", "((a + b) - c)"},
            std::tuple{"a * b * c", "((a * b) * c)"},
            std::tuple{"a * b / c", "((a * b) / c)"},
            std::tuple{"a + b / c", "(a + (b / c))"},
            std::tuple{"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
            std::tuple{"3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"},
            std::tuple{"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
            std::tuple{"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
            std::tuple{"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
            std::tuple{"true", "true"},
            std::tuple{"false", "false"},
            std::tuple{"3 > 5 == false", "((3 > 5) == false)"},
            std::tuple{"3 < 5 == true", "((3 < 5) == true)"},
            std::tuple{"1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"},
            std::tuple{"(5 + 5) * 2", "((5 + 5) * 2)"},
            std::tuple{"2 / (5 + 5)", "(2 / (5 + 5))"},
            std::tuple{"(5 + 5) * 2 * (5 + 5)", "(((5 + 5) * 2) * (5 + 5))"},
            std::tuple{"-(5 + 5)", "(-(5 + 5))"},
            std::tuple{"!(true == true)", "(!(true == true))"},
            std::tuple{"a + add(b * c) + d", "((a + add((b * c))) + d)"},
            std::tuple{"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))", "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"},
            std::tuple{"add(a + b + c * d / f + g)", "add((((a + b) + ((c * d) / f)) + g))"},
            std::tuple{"a * [1, 2, 3, 4][b * c] * d", "((a * ([1, 2, 3, 4][(b * c)])) * d)"},
            std::tuple{"add(a * b[2], b[1], 2 * [1, 2][1])", "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))"},
        };
        for (auto &[input, expected]: tests) {
            const auto program = createProgram(input);
            BOOST_REQUIRE_EQUAL(program->to_string(), expected);
        }
    }

    BOOST_AUTO_TEST_CASE(testBooleanExpressions) {
        const auto tests = {
            std::tuple{"true", true},
            std::tuple{"false", false},
        };

        for (auto &[input, expected_value]: tests) {
            const auto program = createProgram(input);
            countStatements(1, *program);
            const auto expression_statement = dynamic_cast<ExpressionStatement *>(program->statements[0]);
            testBooleanLiteral(expression_statement->expression, expected_value);
        }
    }

    BOOST_AUTO_TEST_CASE(testIfExpression) {
        const auto input = "if (x < y) {x}";
        const auto program = createProgram(input);
        countStatements(1, *program);
        const auto expression_statement = dynamic_cast<ExpressionStatement *>(program->statements[0]);
        process(expression_statement->expression, [](Statement *st) {
            const auto exp = dynamic_cast<IfExpression *>(st);
            testInfixExpression(exp->condition, "x", "<", "y");
            testBlockStatement(exp->consequence, "x");
            BOOST_REQUIRE(exp->alternative == std::nullopt);
        });
    }

    BOOST_AUTO_TEST_CASE(testIfElseExpression) {
        const auto input = "if (x < y) {x} else {y}";
        const auto program = createProgram(input);
        countStatements(1, *program);
        const auto expression_statement = dynamic_cast<ExpressionStatement *>(program->statements[0]);
        process(expression_statement->expression, [](Statement *st) {
            const auto exp = dynamic_cast<IfExpression *>(st);
            testInfixExpression(exp->condition, "x", "<", "y");
            testBlockStatement(exp->consequence, "x");
            testBlockStatement(exp->alternative, "y");
        });
    }

    BOOST_AUTO_TEST_CASE(testFunctionLiteral) {
        const auto input = "fn(x, y) { x + y;}";
        const auto program = createProgram(input);
        countStatements(1, *program);
        const auto expression_statement = dynamic_cast<ExpressionStatement *>(program->statements[0]);
        process(expression_statement->expression, [](Statement *st) {
            const auto function = dynamic_cast<FunctionLiteral *>(st);
            if (const auto opt_parameters = function->parameters; opt_parameters.has_value()) {
                const auto parameters = opt_parameters.value();
                testLiteralExpression(parameters[0], "x");
                testLiteralExpression(parameters[1], "y");
            } else {
                BOOST_FAIL("empty parameters");
            }
            processT(function->body, [](const BlockStatement *body) {
                if (const auto opt_statements = body->statements; opt_statements.has_value()) {
                    const auto statements = opt_statements.value();
                    BOOST_REQUIRE_EQUAL(1, statements.size());
                    process(statements[0], [](Statement *sts) {
                        const auto body_statement = dynamic_cast<ExpressionStatement *>(sts);
                        testInfixExpression(body_statement->expression, "x", "+", "y");
                    });
                } else {
                    BOOST_FAIL("empty body");
                }
            });
        });
    }

BOOST_AUTO_TEST_SUITE_END()
