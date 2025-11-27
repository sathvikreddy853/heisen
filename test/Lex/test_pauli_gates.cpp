#include <gtest/gtest.h>
#include <Test.hpp>

TEST(PauliGates, I) { EXPECT_EQ(run_lexer("I"), GATE_I); }
TEST(PauliGates, X) { EXPECT_EQ(run_lexer("X"), GATE_X); }
TEST(PauliGates, Y) { EXPECT_EQ(run_lexer("Y"), GATE_Y); }
TEST(PauliGates, Z) { EXPECT_EQ(run_lexer("Z"), GATE_Z); }
