#include <gtest/gtest.h>
#include <Test.hpp>

TEST(IdentifierTest, Simple)      { EXPECT_EQ(run_lexer("abc"), IDENTIFIER); }
TEST(IdentifierTest, WithDigits)  { EXPECT_EQ(run_lexer("abc123"), IDENTIFIER); }
TEST(IdentifierTest, Underscore)  { EXPECT_EQ(run_lexer("_x"), IDENTIFIER); }
