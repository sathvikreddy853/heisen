#include <gtest/gtest.h>
#include "Test.hpp"

TEST(ParserInvalid, MissingParenInFunction) {
    const char* code =
        "func add(x: int { "
        "    return x; "
        "}";

    bool ok = parse_string(code);

    EXPECT_FALSE(ok) << "Parser should fail when function header is missing ')'";
}
