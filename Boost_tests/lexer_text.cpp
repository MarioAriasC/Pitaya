#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "lexer.h"
#include "tokens.h"
#include <iostream>

BOOST_AUTO_TEST_SUITE(Lexer_suite)
    BOOST_AUTO_TEST_CASE(testValidateLexer) {
        const std::string code = "let five = 5;\n"
                "let ten = 10;\n"
                "\n"
                "let add = fn(x, y) {\n"
                "	x + y;\n"
                "}\n"
                "\n"
                "let result = add(five, ten);\n"
                "!-/*5;\n"
                "5 < 10 > 5;\n"
                "\n"
                "if (5 < 10) {\n"
                "	return true;\n"
                "} else {\n"
                "	return false;\n"
                "}\n"
                "\n"
                "10 == 10;\n"
                "10 != 9;   \n"
                "\"foobar\"\n"
                "\"foo bar\"     \n"
                "[1,2];\n"
                "{\"foo\":\"bar\"}";
        const auto lexer = new Lexer(code);
        const auto expected = {
            std::pair{TokenType::LET, "let"},
            std::pair{TokenType::IDENT, "five"},
            std::pair{TokenType::ASSIGN, "="},
            std::pair{TokenType::INT, "5"},
            std::pair{TokenType::SEMICOLON, ";"},
            std::pair{TokenType::LET, "let"},
            std::pair{TokenType::IDENT, "ten"},
            std::pair{TokenType::ASSIGN, "="},
            std::pair{TokenType::INT, "10"},
            std::pair{TokenType::SEMICOLON, ";"},
            std::pair{TokenType::LET, "let"},
            std::pair{TokenType::IDENT, "add"},
            std::pair{TokenType::ASSIGN, "="},
            std::pair{TokenType::FUNCTION, "fn"},
            std::pair{TokenType::LPAREN, "("},
            std::pair{TokenType::IDENT, "x"},
            std::pair{TokenType::COMMA, ","},
            std::pair{TokenType::IDENT, "y"},
            std::pair{TokenType::RPAREN, ")"},
            std::pair{TokenType::LBRACE, "{"},
            std::pair{TokenType::IDENT, "x"},
            std::pair{TokenType::PLUS, "+"},
            std::pair{TokenType::IDENT, "y"},
            std::pair{TokenType::SEMICOLON, ";"},
            std::pair{TokenType::RBRACE, "}"},
            std::pair{TokenType::LET, "let"},
            std::pair{TokenType::IDENT, "result"},
            std::pair{TokenType::ASSIGN, "="},
            std::pair{TokenType::IDENT, "add"},
            std::pair{TokenType::LPAREN, "("},
            std::pair{TokenType::IDENT, "five"},
            std::pair{TokenType::COMMA, ","},
            std::pair{TokenType::IDENT, "ten"},
            std::pair{TokenType::RPAREN, ")"},
            std::pair{TokenType::SEMICOLON, ";"},
            std::pair{TokenType::BANG, "!"},
            std::pair{TokenType::MINUS, "-"},
            std::pair{TokenType::SLASH, "/"},
            std::pair{TokenType::ASTERISK, "*"},
            std::pair{TokenType::INT, "5"},
            std::pair{TokenType::SEMICOLON, ";"},
            std::pair{TokenType::INT, "5"},
            std::pair{TokenType::LT, "<"},
            std::pair{TokenType::INT, "10"},
            std::pair{TokenType::GT, ">"},
            std::pair{TokenType::INT, "5"},
            std::pair{TokenType::SEMICOLON, ";"},
            std::pair{TokenType::IF, "if"},
            std::pair{TokenType::LPAREN, "("},
            std::pair{TokenType::INT, "5"},
            std::pair{TokenType::LT, "<"},
            std::pair{TokenType::INT, "10"},
            std::pair{TokenType::RPAREN, ")"},
            std::pair{TokenType::LBRACE, "{"},
            std::pair{TokenType::RETURN, "return"},
            std::pair{TokenType::TRUE, "true"},
            std::pair{TokenType::SEMICOLON, ";"},
            std::pair{TokenType::RBRACE, "}"},
            std::pair{TokenType::ELSE, "else"},
            std::pair{TokenType::LBRACE, "{"},
            std::pair{TokenType::RETURN, "return"},
            std::pair{TokenType::FALSE, "false"},
            std::pair{TokenType::SEMICOLON, ";"},
            std::pair{TokenType::RBRACE, "}"},
            std::pair{TokenType::INT, "10"},
            std::pair{TokenType::EQ, "=="},
            std::pair{TokenType::INT, "10"},
            std::pair{TokenType::SEMICOLON, ";"},
            std::pair{TokenType::INT, "10"},
            std::pair{TokenType::NOT_EQ, "!="},
            std::pair{TokenType::INT, "9"},
            std::pair{TokenType::SEMICOLON, ";"},
            std::pair{TokenType::STRING, "foobar"},
            std::pair{TokenType::STRING, "foo bar"},
            std::pair{TokenType::LBRACKET, "["},
            std::pair{TokenType::INT, "1"},
            std::pair{TokenType::COMMA, ","},
            std::pair{TokenType::INT, "2"},
            std::pair{TokenType::RBRACKET, "]"},
            std::pair{TokenType::SEMICOLON, ";"},
            std::pair{TokenType::LBRACE, "{"},
            std::pair{TokenType::STRING, "foo"},
            std::pair{TokenType::COLON, ":"},
            std::pair{TokenType::STRING, "bar"},
            std::pair{TokenType::RBRACE, "}"},
            std::pair{TokenType::EOF_, ""},
        };

        for (const auto &[fst, snd]: expected) {
            const auto token = lexer->nextToken();
            BOOST_REQUIRE(token->tokenType == fst);
            BOOST_REQUIRE(token->literal == snd);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
