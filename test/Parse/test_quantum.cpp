#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Quantum, TensoredState) {
    auto* expr = dynamic_cast<QuantumStateList*>(parseExpr("[q1, q2];"));
    ASSERT_NE(expr, nullptr);
    EXPECT_EQ(expr->getStates().size(), 2);
}
