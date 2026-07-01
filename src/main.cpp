#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "benchmark.h"
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

void print_usage(std::ostream& os) {
    os << "GPUVision\n";
    os << "Usage:\n";
    os << "  gpuvision --demo\n";
    os << "  gpuvision --benchmark --warps 64 --instructions 50 "
          "--memory-probability 0.25 --memory-latency 20 --sms 4 --seed 42\n";
    os << "  gpuvision --benchmark --export results.csv\n";
}

std::string require_value(int& index, int argc, char* argv[]) {
    if (index + 1 >= argc) {
        throw std::invalid_argument(std::string("Missing value for ") + argv[index]);
    }
    ++index;
    return argv[index];
}

int parse_int_option(int& index, int argc, char* argv[]) {
    return std::stoi(require_value(index, argc, argv));
}

double parse_double_option(int& index, int argc, char* argv[]) {
    return std::stod(require_value(index, argc, argv));
}

std::uint32_t parse_seed_option(int& index, int argc, char* argv[]) {
    const unsigned long seed = std::stoul(require_value(index, argc, argv));
    return static_cast<std::uint32_t>(seed);
}

int run_demo() {
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

int run_benchmark_mode(const gpuvision::BenchmarkConfig& config,
                       const std::string& export_path) {
    const std::vector<gpuvision::BenchmarkResult> results = gpuvision::run_benchmark(config);
    gpuvision::print_benchmark_results(config, results, std::cout);

    if (!export_path.empty()) {
        gpuvision::export_benchmark_csv(results, export_path);
        std::cout << "CSV exported to " << export_path << '\n';
    }

    return 0;
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        bool benchmark_mode = false;
        bool demo_mode = argc == 1;
        std::string export_path;

        gpuvision::BenchmarkConfig benchmark_config;
        benchmark_config.workload.numWarps = 64;
        benchmark_config.workload.instructionsPerWarp = 50;
        benchmark_config.workload.memoryInstructionProbability = 0.25;
        benchmark_config.workload.memoryLatency = 20;
        benchmark_config.workload.randomSeed = 42;
        benchmark_config.smCount = 4;

        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];

            if (arg == "--help" || arg == "-h") {
                print_usage(std::cout);
                return 0;
            }
            if (arg == "--demo") {
                demo_mode = true;
                continue;
            }
            if (arg == "--benchmark") {
                benchmark_mode = true;
                demo_mode = false;
                continue;
            }
            if (arg == "--warps") {
                benchmark_config.workload.numWarps = parse_int_option(i, argc, argv);
                continue;
            }
            if (arg == "--instructions") {
                benchmark_config.workload.instructionsPerWarp = parse_int_option(i, argc, argv);
                continue;
            }
            if (arg == "--memory-probability") {
                benchmark_config.workload.memoryInstructionProbability =
                    parse_double_option(i, argc, argv);
                continue;
            }
            if (arg == "--memory-latency") {
                benchmark_config.workload.memoryLatency = parse_int_option(i, argc, argv);
                continue;
            }
            if (arg == "--sms") {
                benchmark_config.smCount = parse_int_option(i, argc, argv);
                continue;
            }
            if (arg == "--seed") {
                benchmark_config.workload.randomSeed = parse_seed_option(i, argc, argv);
                continue;
            }
            if (arg == "--export") {
                export_path = require_value(i, argc, argv);
                continue;
            }

            throw std::invalid_argument("Unknown argument: " + arg);
        }

        if (benchmark_mode) {
            return run_benchmark_mode(benchmark_config, export_path);
        }
        if (demo_mode) {
            return run_demo();
        }

        print_usage(std::cerr);
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n\n";
        print_usage(std::cerr);
        return 1;
    }
}
