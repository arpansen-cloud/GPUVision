#include <algorithm>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "benchmark.h"
#include "engine.h"
#include "json_export.h"
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
    os << "  gpuvision --demo [--json] [--export demo.json]\n";
    os << "  gpuvision --benchmark --warps 64 --instructions 50 "
          "--memory-probability 0.25 --memory-latency 20 --sms 4 --seed 42 [--json]\n";
    os << "  gpuvision --benchmark --export results.csv\n";
    os << "  gpuvision --benchmark --json --export results.json\n";
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

int max_instructions_per_warp(const std::vector<gpuvision::Warp>& warps) {
    std::size_t max_count = 0;
    for (const gpuvision::Warp& warp : warps) {
        max_count = std::max(max_count, warp.instruction_count());
    }
    return static_cast<int>(max_count);
}

int run_demo(bool json_mode, const std::string& export_path) {
    const std::vector<gpuvision::Warp> demo_warps = create_demo_warps();
    const gpuvision::EngineConfig config{
        2,
        4,
        gpuvision::SchedulerType::ROUND_ROBIN,
        !json_mode,
        json_mode
    };

    gpuvision::Engine engine(config, demo_warps);
    const gpuvision::Metrics& metrics = engine.run(json_mode ? nullptr : &std::cout);

    if (json_mode) {
        gpuvision::SimulationRunResult result;
        result.mode = "demo";
        result.scheduler = gpuvision::scheduler_type_name(config.scheduler_type);
        result.sms = config.sm_count;
        result.warps = static_cast<int>(demo_warps.size());
        result.instructions_per_warp = max_instructions_per_warp(demo_warps);
        result.memory_latency = config.memory_latency;
        result.metrics = metrics;
        result.timeline = engine.timeline();

        const std::string json = gpuvision::simulation_run_to_json(result);
        if (!export_path.empty()) {
            gpuvision::write_json_file(export_path, json);
        } else {
            std::cout << json;
        }
        return 0;
    }

    std::cout << '\n' << metrics;
    return 0;
}

int run_benchmark_mode(const gpuvision::BenchmarkConfig& config,
                       const std::string& export_path,
                       bool json_mode) {
    const std::vector<gpuvision::BenchmarkResult> results = gpuvision::run_benchmark(config);
    if (json_mode) {
        const std::string json = gpuvision::benchmark_to_json(config, results);
        if (!export_path.empty()) {
            gpuvision::write_json_file(export_path, json);
        } else {
            std::cout << json;
        }
        return 0;
    }

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
        bool json_mode = false;
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
            if (arg == "--json") {
                json_mode = true;
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
            return run_benchmark_mode(benchmark_config, export_path, json_mode);
        }
        if (demo_mode) {
            return run_demo(json_mode, export_path);
        }

        print_usage(std::cerr);
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n\n";
        print_usage(std::cerr);
        return 1;
    }
}
