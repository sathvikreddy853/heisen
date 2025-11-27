#include <gtest/gtest.h>
#include <Test.hpp>

TEST(ControlledGates, CNOT)  { EXPECT_EQ(run_lexer("CNOT"), GATE_CNOT); }
TEST(ControlledGates, CZ)    { EXPECT_EQ(run_lexer("CZ"), GATE_CZ); }
TEST(ControlledGates, SWAP)  { EXPECT_EQ(run_lexer("SWAP"), GATE_SWAP); }
TEST(ControlledGates, CSWAP) { EXPECT_EQ(run_lexer("CSWAP"), GATE_CSWAP); }
TEST(ControlledGates, CCNOT) { EXPECT_EQ(run_lexer("CCNOT"), GATE_CCNOT); }
