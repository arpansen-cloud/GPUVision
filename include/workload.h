#ifndef GPUVISION_WORKLOAD_H
#define GPUVISION_WORKLOAD_H

#include <cstdint>
#include <vector>

#include "warp.h"

namespace gpuvision {

struct WorkloadConfig {
    int numWarps = 8;
    int instructionsPerWarp = 8;
    double memoryInstructionProbability = 0.25;
    int memoryLatency = 4;
    std::uint32_t randomSeed = 42;
};

std::vector<Warp> generate_workload(const WorkloadConfig& config);
bool workloads_equal(const std::vector<Warp>& left, const std::vector<Warp>& right);

} // namespace gpuvision

#endif // GPUVISION_WORKLOAD_H
