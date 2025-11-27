#include <gtest/gtest.h>
#include <Test.hpp>

TEST(PunctTest, LBracket) { EXPECT_EQ(run_lexer("["), '['); }
TEST(PunctTest, RBracket) { EXPECT_EQ(run_lexer("]"), ']'); }
TEST(PunctTest, LBrace)   { EXPECT_EQ(run_lexer("{"), '{'); }
TEST(PunctTest, RBrace)   { EXPECT_EQ(run_lexer("}"), '}'); }
TEST(PunctTest, LParen)   { EXPECT_EQ(run_lexer("("), '('); }
TEST(PunctTest, RParen)   { EXPECT_EQ(run_lexer(")"), ')'); }
TEST(PunctTest, Semi)     { EXPECT_EQ(run_lexer(";"), ';'); }
TEST(PunctTest, Colon)    { EXPECT_EQ(run_lexer(":"), ':'); }
TEST(PunctTest, Comma)    { EXPECT_EQ(run_lexer(","), ','); }
TEST(PunctTest, Dot)      { EXPECT_EQ(run_lexer("."), '.'); }
