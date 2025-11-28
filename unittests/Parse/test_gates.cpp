#include <gtest/gtest.h>
#include "Test.hpp"

TEST(Gates, H_Gate) {
    auto* stmt = dynamic_cast<ApplyGateStmt*>(parseStmt("H @ q;"));
    ASSERT_NE(stmt, nullptr);

    auto* gate = dynamic_cast<SimpleGateNode*>(stmt->getGate());
    ASSERT_NE(gate, nullptr);
    EXPECT_EQ(gate->getGateKind(), GateKind::H);
}
