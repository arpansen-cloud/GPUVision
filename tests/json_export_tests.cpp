#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "benchmark.h"
#include "engine.h"
#include "json_export.h"
#include "warp.h"
#include "workload.h"

namespace {

gpuvision::Instruction compute() {
    return {gpuvision::InstructionType::COMPUTE};
}

gpuvision::Instruction memory() {
    return {gpuvision::InstructionType::MEMORY};
}

gpuvision::SimulationRunResult make_demo_json_result() {
    std::vector<gpuvision::Warp> warps{
        gpuvision::Warp(0, {compute(), memory(), compute()}),
        gpuvision::Warp(1, {compute(), compute()})
    };
    const int warp_count = static_cast<int>(warps.size());

    gpuvision::EngineConfig config;
    config.sm_count = 2;
    config.memory_latency = 2;
    config.scheduler_type = gpuvision::SchedulerType::ROUND_ROBIN;
    config.capture_timeline = true;

    gpuvision::Engine engine(config, warps);
    const gpuvision::Metrics metrics = engine.run();

    gpuvision::SimulationRunResult result;
    result.mode = "demo";
    result.scheduler = gpuvision::scheduler_type_name(config.scheduler_type);
    result.sms = config.sm_count;
    result.warps = warp_count;
    result.instructions_per_warp = 3;
    result.memory_latency = config.memory_latency;
    result.metrics = metrics;
    result.timeline = engine.timeline();
    return result;
}

gpuvision::BenchmarkConfig small_benchmark_config() {
    gpuvision::BenchmarkConfig config;
    config.workload.numWarps = 5;
    config.workload.instructionsPerWarp = 6;
    config.workload.memoryInstructionProbability = 0.3;
    config.workload.memoryLatency = 3;
    config.workload.randomSeed = 7;
    config.smCount = 2;
    return config;
}

} // namespace

TEST(JsonExportTests, DemoJsonOutputContainsRequiredFields) {
    const std::string json = gpuvision::simulation_run_to_json(make_demo_json_result());

    EXPECT_NE(json.find("\"mode\": \"demo\""), std::string::npos);
    EXPECT_NE(json.find("\"scheduler\": \"Round Robin\""), std::string::npos);
    EXPECT_NE(json.find("\"config\""), std::string::npos);
    EXPECT_NE(json.find("\"final_metrics\""), std::string::npos);
    EXPECT_NE(json.find("\"timeline\""), std::string::npos);
    EXPECT_EQ(json.front(), '{');
    EXPECT_NE(json.find("}\n"), std::string::npos);
}

TEST(JsonExportTests, BenchmarkJsonOutputContainsAllSchedulers) {
    const gpuvision::BenchmarkConfig config = small_benchmark_config();
    const std::vector<gpuvision::BenchmarkResult> results = gpuvision::run_benchmark(config);
    const std::string json = gpuvision::benchmark_to_json(config, results);

    EXPECT_NE(json.find("\"mode\": \"benchmark\""), std::string::npos);
    EXPECT_NE(json.find("\"scheduler\": \"Round Robin\""), std::string::npos);
    EXPECT_NE(json.find("\"scheduler\": \"Oldest First\""), std::string::npos);
    EXPECT_NE(json.find("\"scheduler\": \"Greedy Then Oldest\""), std::string::npos);
}

TEST(JsonExportTests, JsonExportFileIsCreated) {
    const std::string path = "gpuvision_json_export_test.json";
    const std::string json = gpuvision::simulation_run_to_json(make_demo_json_result());

    gpuvision::write_json_file(path, json);

    std::ifstream in(path);
    ASSERT_TRUE(in);
    std::string first_line;
    std::getline(in, first_line);
    EXPECT_EQ(first_line, "{");

    in.close();
    std::remove(path.c_str());
}

TEST(JsonExportTests, TimelineContainsCycleNumbers) {
    const std::string json = gpuvision::simulation_run_to_json(make_demo_json_result());

    EXPECT_NE(json.find("\"cycle\": 0"), std::string::npos);
}

TEST(JsonExportTests, SmEventsIncludeSmId) {
    const std::string json = gpuvision::simulation_run_to_json(make_demo_json_result());

    EXPECT_NE(json.find("\"sm_id\": 0"), std::string::npos);
}

TEST(JsonExportTests, JsonModeDoesNotBreakBenchmarkBehavior) {
    const gpuvision::BenchmarkConfig config = small_benchmark_config();
    const std::vector<gpuvision::BenchmarkResult> results = gpuvision::run_benchmark(config);
    const std::string json = gpuvision::benchmark_to_json(config, results);

    ASSERT_EQ(results.size(), 3U);
    EXPECT_NE(json.find("\"results\""), std::string::npos);
    for (const gpuvision::BenchmarkResult& result : results) {
        EXPECT_EQ(result.metrics.completed_warps, config.workload.numWarps);
    }
}

TEST(JsonExportTests, BenchmarkJsonIsDeterministicForSameSeed) {
    const gpuvision::BenchmarkConfig config = small_benchmark_config();

    const std::string first =
        gpuvision::benchmark_to_json(config, gpuvision::run_benchmark(config));
    const std::string second =
        gpuvision::benchmark_to_json(config, gpuvision::run_benchmark(config));

    EXPECT_EQ(first, second);
}

TEST(JsonExportTests, CsvExportStillWorksAfterJsonChanges) {
    const gpuvision::BenchmarkConfig config = small_benchmark_config();
    const std::vector<gpuvision::BenchmarkResult> results = gpuvision::run_benchmark(config);
    const std::string path = "gpuvision_json_csv_regression_test.csv";

    gpuvision::export_benchmark_csv(results, path);

    std::ifstream in(path);
    ASSERT_TRUE(in);
    std::string header;
    std::getline(in, header);
    EXPECT_NE(header.find("scheduler,total_cycles"), std::string::npos);

    in.close();
    std::remove(path.c_str());
}
