#include <gtest/gtest.h>
#include <Test.hpp>

TEST(ArithmeticOps, Plus)  { EXPECT_EQ(run_lexer("+"), '+'); }
TEST(ArithmeticOps, Minus) { EXPECT_EQ(run_lexer("-"), '-'); }
TEST(ArithmeticOps, Mul)   { EXPECT_EQ(run_lexer("*"), '*'); }
TEST(ArithmeticOps, Div)   { EXPECT_EQ(run_lexer("/"), '/'); }
TEST(ArithmeticOps, Mod)   { EXPECT_EQ(run_lexer("%"), '%'); }
TEST(ArithmeticOps, Exp)   { EXPECT_EQ(run_lexer("**"), EXP); }
