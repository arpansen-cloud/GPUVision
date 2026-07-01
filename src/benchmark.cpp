#include "benchmark.h"

#include <fstream>
#include <iomanip>
#include <ostream>
#include <stdexcept>

#include "engine.h"

namespace gpuvision {

namespace {

std::vector<SchedulerType> benchmark_schedulers() {
    return {
        SchedulerType::ROUND_ROBIN,
        SchedulerType::OLDEST_FIRST,
        SchedulerType::GREEDY_THEN_OLDEST
    };
}

void print_metric_block(const BenchmarkResult& result, std::ostream& os) {
    os << result.schedulerName << '\n';
    os << "  Total cycles: " << result.metrics.total_cycles << '\n';
    os << "  Instructions completed: " << result.metrics.instructions_completed << '\n';
    os << "  IPC: " << std::fixed << std::setprecision(2) << result.metrics.ipc() << '\n';
    os << "  SM utilization: " << std::fixed << std::setprecision(2)
       << result.metrics.sm_utilization_percent() << "%\n";
    os << "  Idle cycles: " << result.metrics.idle_cycles << '\n';
    os << "  Memory stall count: " << result.metrics.memory_stall_count << '\n';
    os << "  Completed warps: " << result.metrics.completed_warps << '\n';
    os << "  Average warp completion time: " << std::fixed << std::setprecision(2)
       << result.metrics.average_warp_completion_time << "\n\n";
}

} // namespace

std::vector<BenchmarkResult> run_benchmark(const BenchmarkConfig& config) {
    return run_benchmark(config, generate_workload(config.workload));
}

std::vector<BenchmarkResult> run_benchmark(const BenchmarkConfig& config,
                                           const std::vector<Warp>& base_workload) {
    if (config.smCount <= 0) {
        throw std::invalid_argument("SM count must be positive");
    }

    std::vector<BenchmarkResult> results;
    results.reserve(benchmark_schedulers().size());

    for (SchedulerType scheduler_type : benchmark_schedulers()) {
        EngineConfig engine_config;
        engine_config.sm_count = config.smCount;
        engine_config.memory_latency = config.workload.memoryLatency;
        engine_config.scheduler_type = scheduler_type;
        engine_config.enable_cycle_logs = false;

        Engine engine(engine_config, base_workload);
        const Metrics metrics = engine.run();

        results.push_back({scheduler_type, scheduler_type_name(scheduler_type), metrics});
    }

    return results;
}

void print_benchmark_results(const BenchmarkConfig& config,
                             const std::vector<BenchmarkResult>& results,
                             std::ostream& os) {
    os << "Benchmark Results\n";
    os << "Workload:\n";
    os << "  Warps: " << config.workload.numWarps << '\n';
    os << "  Instructions per warp: " << config.workload.instructionsPerWarp << '\n';
    os << "  Memory probability: " << config.workload.memoryInstructionProbability << '\n';
    os << "  Memory latency: " << config.workload.memoryLatency << '\n';
    os << "  SMs: " << config.smCount << "\n\n";
    os << "Scheduler Comparison:\n";

    for (const BenchmarkResult& result : results) {
        print_metric_block(result, os);
    }
}

void export_benchmark_csv(const std::vector<BenchmarkResult>& results,
                          const std::string& path) {
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("Could not open CSV export path: " + path);
    }

    out << "scheduler,total_cycles,instructions_completed,ipc,sm_utilization,"
           "idle_cycles,memory_stalls,completed_warps,average_warp_completion_time\n";

    for (const BenchmarkResult& result : results) {
        out << result.schedulerName << ','
            << result.metrics.total_cycles << ','
            << result.metrics.instructions_completed << ','
            << std::fixed << std::setprecision(6) << result.metrics.ipc() << ','
            << std::fixed << std::setprecision(6) << result.metrics.sm_utilization_percent() << ','
            << result.metrics.idle_cycles << ','
            << result.metrics.memory_stall_count << ','
            << result.metrics.completed_warps << ','
            << std::fixed << std::setprecision(6)
            << result.metrics.average_warp_completion_time << '\n';
    }
}

std::string scheduler_type_name(SchedulerType type) {
    switch (type) {
        case SchedulerType::ROUND_ROBIN:
            return "Round Robin";
        case SchedulerType::OLDEST_FIRST:
            return "Oldest First";
        case SchedulerType::GREEDY_THEN_OLDEST:
            return "Greedy Then Oldest";
    }
    return "Unknown";
}

} // namespace gpuvision
