#ifndef GPUVISION_BENCHMARK_H
#define GPUVISION_BENCHMARK_H

#include <iosfwd>
#include <string>
#include <vector>

#include "metrics.h"
#include "scheduler.h"
#include "workload.h"

namespace gpuvision {

struct BenchmarkConfig {
    WorkloadConfig workload;
    int smCount = 4;
};

struct BenchmarkResult {
    SchedulerType schedulerType;
    std::string schedulerName;
    Metrics metrics;
};

std::vector<BenchmarkResult> run_benchmark(const BenchmarkConfig& config);
std::vector<BenchmarkResult> run_benchmark(const BenchmarkConfig& config,
                                           const std::vector<Warp>& base_workload);
void print_benchmark_results(const BenchmarkConfig& config,
                             const std::vector<BenchmarkResult>& results,
                             std::ostream& os);
void export_benchmark_csv(const std::vector<BenchmarkResult>& results,
                          const std::string& path);
std::string scheduler_type_name(SchedulerType type);

} // namespace gpuvision

#endif // GPUVISION_BENCHMARK_H
