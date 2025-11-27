#include <gtest/gtest.h>
#include <Test.hpp>

TEST(CompOps, Eq)      { EXPECT_EQ(run_lexer("=="), EQ_OP); }
TEST(CompOps, Ne)      { EXPECT_EQ(run_lexer("!="), NE_OP); }
TEST(CompOps, Ge)      { EXPECT_EQ(run_lexer(">="), GE_OP); }
TEST(CompOps, Le)      { EXPECT_EQ(run_lexer("<="), LE_OP); }
TEST(CompOps, Greater) { EXPECT_EQ(run_lexer(">"), '>'); }
TEST(CompOps, Less)    { EXPECT_EQ(run_lexer("<"), '<'); }
