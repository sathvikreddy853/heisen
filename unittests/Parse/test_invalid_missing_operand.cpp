#include <gtest/gtest.h>
#include "Test.hpp"

TEST(ParserInvalid, MissingOperand) {
    const char* code = "1 +";

    bool ok = parse_string(code);   // Your TestP helper returns success/failure

    EXPECT_FALSE(ok) << "Parser should fail on incomplete binary expression";
}
