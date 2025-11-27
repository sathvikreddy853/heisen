#include <gtest/gtest.h>
#include <Test.hpp>

TEST(LiteralTest, IntLiteral)   { EXPECT_EQ(run_lexer("123"), INT_LITERAL); }
TEST(LiteralTest, FloatLiteral) { EXPECT_EQ(run_lexer("3.14"), FLOAT_LITERAL); }
TEST(LiteralTest, StringLiteral){ EXPECT_EQ(run_lexer("\"abc\""), STRING_LITERAL); }
