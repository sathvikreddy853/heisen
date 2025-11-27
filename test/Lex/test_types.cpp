#include <gtest/gtest.h>
#include <Test.hpp>

TEST(TypeTest, Qubit)  { EXPECT_EQ(run_lexer("qubit"), QUBIT); }
TEST(TypeTest, Bit)    { EXPECT_EQ(run_lexer("bit"), BIT); }
TEST(TypeTest, Int)    { EXPECT_EQ(run_lexer("int"), INT); }
TEST(TypeTest, Float)  { EXPECT_EQ(run_lexer("float"), FLOAT); }
TEST(TypeTest, String) { EXPECT_EQ(run_lexer("string"), STRING); }
TEST(TypeTest, Bool)   { EXPECT_EQ(run_lexer("bool"), BOOL); }