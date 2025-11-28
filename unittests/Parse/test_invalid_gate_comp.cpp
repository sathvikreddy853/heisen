#include <gtest/gtest.h>
#include "Test.hpp"

TEST(ParserInvalid, InvalidGateComposition) {
    const char* code = "H @@ X @ q0;";

    bool ok = parse_string(code);

    EXPECT_FALSE(ok) << "Parser should reject double '@' in gate composition";
}
