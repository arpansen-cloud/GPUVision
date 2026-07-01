#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "benchmark.h"
#include "workload.h"

namespace {

gpuvision::BenchmarkConfig small_benchmark_config() {
    gpuvision::BenchmarkConfig config;
    config.workload.numWarps = 6;
    config.workload.instructionsPerWarp = 8;
    config.workload.memoryInstructionProbability = 0.25;
    config.workload.memoryLatency = 3;
    config.workload.randomSeed = 99;
    config.smCount = 2;
    return config;
}

} // namespace

TEST(BenchmarkTests, BenchmarkRunsAllSchedulers) {
    const gpuvision::BenchmarkConfig config = small_benchmark_config();

    const std::vector<gpuvision::BenchmarkResult> results = gpuvision::run_benchmark(config);

    ASSERT_EQ(results.size(), 3U);
    EXPECT_EQ(results[0].schedulerName, "Round Robin");
    EXPECT_EQ(results[1].schedulerName, "Oldest First");
    EXPECT_EQ(results[2].schedulerName, "Greedy Then Oldest");
}

TEST(BenchmarkTests, CsvExportCreatesExpectedHeaders) {
    const gpuvision::BenchmarkConfig config = small_benchmark_config();
    const std::vector<gpuvision::BenchmarkResult> results = gpuvision::run_benchmark(config);
    const std::string path = "gpuvision_benchmark_test.csv";

    gpuvision::export_benchmark_csv(results, path);

    std::ifstream in(path);
    ASSERT_TRUE(in);

    std::string header;
    std::getline(in, header);
    EXPECT_EQ(header,
              "scheduler,total_cycles,instructions_completed,ipc,sm_utilization,"
              "idle_cycles,memory_stalls,completed_warps,average_warp_completion_time");

    in.close();
    std::remove(path.c_str());
}

TEST(BenchmarkTests, AverageWarpCompletionTimeIsNonNegative) {
    const gpuvision::BenchmarkConfig config = small_benchmark_config();

    const std::vector<gpuvision::BenchmarkResult> results = gpuvision::run_benchmark(config);

    for (const gpuvision::BenchmarkResult& result : results) {
        EXPECT_GE(result.metrics.average_warp_completion_time, 0.0);
    }
}

TEST(BenchmarkTests, SmUtilizationRemainsBetweenZeroAndOneHundred) {
    const gpuvision::BenchmarkConfig config = small_benchmark_config();

    const std::vector<gpuvision::BenchmarkResult> results = gpuvision::run_benchmark(config);

    for (const gpuvision::BenchmarkResult& result : results) {
        EXPECT_GE(result.metrics.sm_utilization_percent(), 0.0);
        EXPECT_LE(result.metrics.sm_utilization_percent(), 100.0);
    }
}

TEST(BenchmarkTests, BenchmarkDoesNotMutateBaseWorkloadAcrossSchedulerRuns) {
    const gpuvision::BenchmarkConfig config = small_benchmark_config();
    const std::vector<gpuvision::Warp> base_workload =
        gpuvision::generate_workload(config.workload);
    const std::vector<gpuvision::Warp> snapshot = base_workload;

    const std::vector<gpuvision::BenchmarkResult> results =
        gpuvision::run_benchmark(config, base_workload);

    EXPECT_EQ(results.size(), 3U);
    EXPECT_TRUE(gpuvision::workloads_equal(base_workload, snapshot));
}
