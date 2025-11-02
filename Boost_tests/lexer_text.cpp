#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "lexer.h"
#include "tokens.h"

BOOST_AUTO_TEST_SUITE(Lexer_suite)
BOOST_AUTO_TEST_CASE(testValidateLexer) {
    const std::string code = "";
    const auto lexer = new Lexer(code);
}
BOOST_AUTO_TEST_SUITE_END()