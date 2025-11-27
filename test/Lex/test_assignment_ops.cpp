#include <gtest/gtest.h>
#include <Test.hpp>

TEST(AssignOps, Assign)     { EXPECT_EQ(run_lexer("="), ASSIGN); }
TEST(AssignOps, AddAssign)  { EXPECT_EQ(run_lexer("+="), ADD_ASSIGN); }
TEST(AssignOps, SubAssign)  { EXPECT_EQ(run_lexer("-="), SUB_ASSIGN); }
TEST(AssignOps, MulAssign)  { EXPECT_EQ(run_lexer("*="), MUL_ASSIGN); }
TEST(AssignOps, DivAssign)  { EXPECT_EQ(run_lexer("/="), DIV_ASSIGN); }
TEST(AssignOps, ModAssign)  { EXPECT_EQ(run_lexer("%="), MOD_ASSIGN); }
TEST(AssignOps, ExpAssign)  { EXPECT_EQ(run_lexer("**="), EXP_ASSIGN); }
