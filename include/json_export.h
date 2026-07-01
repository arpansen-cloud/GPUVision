#ifndef GPUVISION_JSON_EXPORT_H
#define GPUVISION_JSON_EXPORT_H

#include <cstddef>
#include <string>
#include <vector>

#include "benchmark.h"
#include "metrics.h"
#include "workload.h"

namespace gpuvision {

struct SmEvent {
    int sm_id = 0;
    int warp_id = -1;
    std::string instruction = "IDLE";
    std::string resulting_state = "IDLE";
    bool idle = true;
};

struct StallEvent {
    int warp_id = 0;
    int before = 0;
    int after = 0;
};

struct WarpStateSnapshot {
    int warp_id = 0;
    std::size_t program_counter = 0;
    std::string state;
    int stall_cycles_remaining = 0;
};

struct CycleEvent {
    int cycle = 0;
    std::vector<SmEvent> sms;
    std::vector<StallEvent> memory_stalls;
    std::vector<int> completed_warps;
    std::vector<WarpStateSnapshot> warp_states;
};

struct MetricsSnapshot {
    int total_cycles = 0;
    int instructions_completed = 0;
    double ipc = 0.0;
    double sm_utilization = 0.0;
    int idle_cycles = 0;
    int memory_stalls = 0;
    int completed_warps = 0;
    double average_warp_completion_time = 0.0;
    int max_warp_completion_time = 0;
    int min_warp_completion_time = 0;
    int total_sm_cycles = 0;
    int non_idle_sm_cycles = 0;
};

struct SimulationRunResult {
    std::string mode = "demo";
    std::string scheduler;
    int sms = 0;
    int warps = 0;
    int instructions_per_warp = 0;
    int memory_latency = 0;
    Metrics metrics;
    std::vector<CycleEvent> timeline;
};

MetricsSnapshot make_metrics_snapshot(const Metrics& metrics);
std::string escape_json_string(const std::string& value);
std::string simulation_run_to_json(const SimulationRunResult& result);
std::string benchmark_to_json(const BenchmarkConfig& config,
                              const std::vector<BenchmarkResult>& results);
void write_json_file(const std::string& path, const std::string& json);

} // namespace gpuvision

#endif // GPUVISION_JSON_EXPORT_H
