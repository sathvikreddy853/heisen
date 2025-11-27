#include <gtest/gtest.h>
#include "Test.hpp"

TEST(CtrlRotGates, CRX) { EXPECT_EQ(run_lexer("CRX"), GATE_CRX); }
TEST(CtrlRotGates, CRY) { EXPECT_EQ(run_lexer("CRY"), GATE_CRY); }
TEST(CtrlRotGates, CRZ) { EXPECT_EQ(run_lexer("CRZ"), GATE_CRZ); }
