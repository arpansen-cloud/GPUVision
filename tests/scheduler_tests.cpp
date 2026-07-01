#include <gtest/gtest.h>

#include "scheduler.h"
#include "warp.h"

namespace {

gpuvision::Instruction compute() {
    return {gpuvision::InstructionType::COMPUTE};
}

gpuvision::Instruction memory() {
    return {gpuvision::InstructionType::MEMORY};
}

} // namespace

TEST(SchedulerTests, RoundRobinOrder) {
    std::vector<gpuvision::Warp> warps{
        gpuvision::Warp(0, {compute(), compute()}),
        gpuvision::Warp(1, {compute(), compute()}),
        gpuvision::Warp(2, {compute(), compute()})
    };
    gpuvision::RoundRobinScheduler scheduler;

    EXPECT_EQ(scheduler.select_warp(warps), 0);
    EXPECT_EQ(scheduler.select_warp(warps), 1);
    EXPECT_EQ(scheduler.select_warp(warps), 2);
    EXPECT_EQ(scheduler.select_warp(warps), 0);
}

TEST(SchedulerTests, StalledWarpsNotScheduled) {
    std::vector<gpuvision::Warp> warps{
        gpuvision::Warp(0, {memory(), compute()}),
        gpuvision::Warp(1, {compute()})
    };
    warps[0].mark_running();
    warps[0].issue_next_instruction(3);

    gpuvision::OldestFirstScheduler scheduler;

    EXPECT_EQ(warps[0].state(), gpuvision::WarpState::STALLED);
    EXPECT_EQ(scheduler.select_warp(warps), 1);
}

TEST(SchedulerTests, CompletedWarpsNotScheduled) {
    std::vector<gpuvision::Warp> warps{
        gpuvision::Warp(0, {compute()}),
        gpuvision::Warp(1, {compute()})
    };
    warps[0].mark_running();
    warps[0].issue_next_instruction(3);

    gpuvision::OldestFirstScheduler scheduler;

    EXPECT_EQ(warps[0].state(), gpuvision::WarpState::COMPLETED);
    EXPECT_EQ(scheduler.select_warp(warps), 1);
}
