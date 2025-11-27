#include <gtest/gtest.h>
#include "Test.hpp"

TEST(BitwiseOps, And)      { EXPECT_EQ(run_lexer("&"), '&'); }
TEST(BitwiseOps, Or)       { EXPECT_EQ(run_lexer("|"), '|'); }
TEST(BitwiseOps, Xor)      { EXPECT_EQ(run_lexer("^"), '^'); }
TEST(BitwiseOps, Not)      { EXPECT_EQ(run_lexer("!"), '!'); }
TEST(BitwiseOps, AndAssign){ EXPECT_EQ(run_lexer("&="), AND_ASSIGN); }
TEST(BitwiseOps, OrAssign) { EXPECT_EQ(run_lexer("|="), OR_ASSIGN); }
TEST(BitwiseOps, XorAssign){ EXPECT_EQ(run_lexer("^="), XOR_ASSIGN); }
TEST(BitwiseOps, RightShift){ EXPECT_EQ(run_lexer(">>"), RIGHT_SHIFT); }
TEST(BitwiseOps, LeftShift) { EXPECT_EQ(run_lexer("<<"), LEFT_SHIFT); }
TEST(BitwiseOps, RightShiftAssign){ EXPECT_EQ(run_lexer(">>="), RIGHT_SHIFT_ASSIGN); }
TEST(BitwiseOps, LeftShiftAssign) { EXPECT_EQ(run_lexer("<<="), LEFT_SHIFT_ASSIGN); }
