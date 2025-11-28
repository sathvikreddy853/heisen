#include <gtest/gtest.h>
#include "Test.hpp"

TEST(RotationGates, RX) { EXPECT_EQ(run_lexer("RX"), GATE_RX); }
TEST(RotationGates, RY) { EXPECT_EQ(run_lexer("RY"), GATE_RY); }
TEST(RotationGates, RZ) { EXPECT_EQ(run_lexer("RZ"), GATE_RZ); }
