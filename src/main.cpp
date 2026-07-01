#include <iostream>
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

std::vector<gpuvision::Warp> create_demo_warps() {
    return {
        gpuvision::Warp(0, {compute(), compute(), memory(), compute()}),
        gpuvision::Warp(1, {compute(), memory(), compute(), compute()}),
        gpuvision::Warp(2, {memory(), compute(), compute()}),
        gpuvision::Warp(3, {compute(), compute(), compute()}),
        gpuvision::Warp(4, {compute(), memory(), memory(), compute()}),
        gpuvision::Warp(5, {memory(), compute(), memory()}),
        gpuvision::Warp(6, {compute(), compute(), memory(), compute()}),
        gpuvision::Warp(7, {compute(), memory(), compute()})
    };
}

} // namespace

int main() {
    const gpuvision::EngineConfig config{
        2,
        4,
        gpuvision::SchedulerType::ROUND_ROBIN,
        true
    };

    gpuvision::Engine engine(config, create_demo_warps());
    const gpuvision::Metrics& metrics = engine.run(&std::cout);

    std::cout << '\n' << metrics;
    return 0;
}
