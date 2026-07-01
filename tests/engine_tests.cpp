#include <gtest/gtest.h>

#include <numeric>
#include <vector>

#include "engine.h"
#include "warp.h"

namespace {

gpuvision::Instruction compute() {
    return {gpuvision::InstructionType::COMPUTE};
}

gpuvision::Instruction memory() {
    return {gpuvision::InstructionType::MEMORY};
}

int total_instructions(const std::vector<gpuvision::Warp>& warps) {
    return std::accumulate(warps.begin(), warps.end(), 0, [](int total, const gpuvision::Warp& warp) {
        return total + static_cast<int>(warp.instruction_count());
    });
}

} // namespace

TEST(EngineTests, MemoryInstructionsCauseCorrectStallDuration) {
    gpuvision::Warp warp(0, {memory(), compute()});

    warp.mark_running();
    const bool caused_stall = warp.issue_next_instruction(3);

    EXPECT_TRUE(caused_stall);
    EXPECT_EQ(warp.state(), gpuvision::WarpState::STALLED);
    EXPECT_EQ(warp.stall_cycles_remaining(), 3);

    warp.tick_stall();
    EXPECT_EQ(warp.state(), gpuvision::WarpState::STALLED);
    EXPECT_EQ(warp.stall_cycles_remaining(), 2);

    warp.tick_stall();
    EXPECT_EQ(warp.state(), gpuvision::WarpState::STALLED);
    EXPECT_EQ(warp.stall_cycles_remaining(), 1);

    warp.tick_stall();
    EXPECT_EQ(warp.state(), gpuvision::WarpState::READY);
    EXPECT_EQ(warp.stall_cycles_remaining(), 0);
}

TEST(EngineTests, SimulationCompletesAllWarps) {
    std::vector<gpuvision::Warp> warps{
        gpuvision::Warp(0, {compute(), memory(), compute()}),
        gpuvision::Warp(1, {memory(), compute()}),
        gpuvision::Warp(2, {compute(), compute()})
    };

    gpuvision::Engine engine({2, 2, gpuvision::SchedulerType::ROUND_ROBIN, false}, warps);
    const gpuvision::Metrics& metrics = engine.run();

    EXPECT_EQ(metrics.completed_warps, 3);
    for (const gpuvision::Warp& warp : engine.warps()) {
        EXPECT_TRUE(warp.is_completed());
    }
}

TEST(EngineTests, InstructionsCompletedNeverExceedsTotalWorkloadInstructions) {
    std::vector<gpuvision::Warp> warps{
        gpuvision::Warp(0, {compute(), memory(), compute()}),
        gpuvision::Warp(1, {memory(), compute(), compute()}),
        gpuvision::Warp(2, {compute()})
    };
    const int workload_instruction_count = total_instructions(warps);

    gpuvision::Engine engine({2, 3, gpuvision::SchedulerType::GREEDY_THEN_OLDEST, false}, warps);
    const gpuvision::Metrics& metrics = engine.run();

    EXPECT_LE(metrics.instructions_completed, workload_instruction_count);
    EXPECT_EQ(metrics.instructions_completed, workload_instruction_count);
}

TEST(EngineTests, SmUtilizationStaysBetweenZeroAndOneHundred) {
    std::vector<gpuvision::Warp> warps{
        gpuvision::Warp(0, {memory(), compute()}),
        gpuvision::Warp(1, {compute(), memory(), compute()})
    };

    gpuvision::Engine engine({2, 5, gpuvision::SchedulerType::OLDEST_FIRST, false}, warps);
    const gpuvision::Metrics& metrics = engine.run();

    EXPECT_GE(metrics.sm_utilization_percent(), 0.0);
    EXPECT_LE(metrics.sm_utilization_percent(), 100.0);
}
