#include "workload.h"

#include <random>
#include <stdexcept>
#include <utility>

namespace gpuvision {

std::vector<Warp> generate_workload(const WorkloadConfig& config) {
    if (config.numWarps < 0) {
        throw std::invalid_argument("Number of warps cannot be negative");
    }
    if (config.instructionsPerWarp < 0) {
        throw std::invalid_argument("Instructions per warp cannot be negative");
    }
    if (config.memoryInstructionProbability < 0.0 ||
        config.memoryInstructionProbability > 1.0) {
        throw std::invalid_argument("Memory instruction probability must be between 0 and 1");
    }
    if (config.memoryLatency < 0) {
        throw std::invalid_argument("Memory latency cannot be negative");
    }

    std::mt19937 rng(config.randomSeed);
    std::bernoulli_distribution memory_distribution(config.memoryInstructionProbability);

    std::vector<Warp> warps;
    warps.reserve(static_cast<std::size_t>(config.numWarps));

    for (int warp_id = 0; warp_id < config.numWarps; ++warp_id) {
        std::vector<Instruction> instructions;
        instructions.reserve(static_cast<std::size_t>(config.instructionsPerWarp));

        for (int instruction = 0; instruction < config.instructionsPerWarp; ++instruction) {
            const bool is_memory = memory_distribution(rng);
            instructions.push_back(
                {is_memory ? InstructionType::MEMORY : InstructionType::COMPUTE});
        }

        warps.emplace_back(warp_id, std::move(instructions));
    }

    return warps;
}

bool workloads_equal(const std::vector<Warp>& left, const std::vector<Warp>& right) {
    if (left.size() != right.size()) {
        return false;
    }

    for (std::size_t i = 0; i < left.size(); ++i) {
        if (left[i].id() != right[i].id()) {
            return false;
        }
        if (left[i].instructions() != right[i].instructions()) {
            return false;
        }
    }

    return true;
}

} // namespace gpuvision
