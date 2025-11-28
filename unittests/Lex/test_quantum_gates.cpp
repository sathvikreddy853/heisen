#include <gtest/gtest.h>
#include "TestL.hpp"

TEST(QuantumGates, H)    { EXPECT_EQ(run_lexer("H"), GATE_H); }
TEST(QuantumGates, S)    { EXPECT_EQ(run_lexer("S"), GATE_S); }
TEST(QuantumGates, T)    { EXPECT_EQ(run_lexer("T"), GATE_T); }
TEST(QuantumGates, CTRL) { EXPECT_EQ(run_lexer("CTRL"), GATE_CTRL); }
