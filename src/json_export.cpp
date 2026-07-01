#include "json_export.h"

#include <fstream>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>

namespace gpuvision {

namespace {

void indent(std::ostringstream& os, int spaces) {
    os << std::string(static_cast<std::size_t>(spaces), ' ');
}

void append_metrics_json(std::ostringstream& os, const Metrics& metrics, int spaces) {
    indent(os, spaces);
    os << "{\n";
    indent(os, spaces + 2);
    os << "\"total_cycles\": " << metrics.total_cycles << ",\n";
    indent(os, spaces + 2);
    os << "\"instructions_completed\": " << metrics.instructions_completed << ",\n";
    indent(os, spaces + 2);
    os << "\"ipc\": " << std::fixed << std::setprecision(6) << metrics.ipc() << ",\n";
    indent(os, spaces + 2);
    os << "\"sm_utilization\": " << std::fixed << std::setprecision(6)
       << metrics.sm_utilization_percent() << ",\n";
    indent(os, spaces + 2);
    os << "\"idle_cycles\": " << metrics.idle_cycles << ",\n";
    indent(os, spaces + 2);
    os << "\"memory_stalls\": " << metrics.memory_stall_count << ",\n";
    indent(os, spaces + 2);
    os << "\"completed_warps\": " << metrics.completed_warps << ",\n";
    indent(os, spaces + 2);
    os << "\"average_warp_completion_time\": " << std::fixed << std::setprecision(6)
       << metrics.average_warp_completion_time << ",\n";
    indent(os, spaces + 2);
    os << "\"max_warp_completion_time\": " << metrics.max_warp_completion_time << ",\n";
    indent(os, spaces + 2);
    os << "\"min_warp_completion_time\": " << metrics.min_warp_completion_time << ",\n";
    indent(os, spaces + 2);
    os << "\"total_sm_cycles\": " << metrics.total_sm_cycles << ",\n";
    indent(os, spaces + 2);
    os << "\"non_idle_sm_cycles\": " << metrics.non_idle_sm_cycles << '\n';
    indent(os, spaces);
    os << '}';
}

void append_string_array(std::ostringstream& os,
                         const std::vector<int>& values,
                         int spaces) {
    indent(os, spaces);
    os << '[';
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            os << ", ";
        }
        os << values[i];
    }
    os << ']';
}

} // namespace

MetricsSnapshot make_metrics_snapshot(const Metrics& metrics) {
    return {
        metrics.total_cycles,
        metrics.instructions_completed,
        metrics.ipc(),
        metrics.sm_utilization_percent(),
        metrics.idle_cycles,
        metrics.memory_stall_count,
        metrics.completed_warps,
        metrics.average_warp_completion_time,
        metrics.max_warp_completion_time,
        metrics.min_warp_completion_time,
        metrics.total_sm_cycles,
        metrics.non_idle_sm_cycles
    };
}

std::string escape_json_string(const std::string& value) {
    std::ostringstream escaped;
    for (const char ch : value) {
        switch (ch) {
            case '"':
                escaped << "\\\"";
                break;
            case '\\':
                escaped << "\\\\";
                break;
            case '\b':
                escaped << "\\b";
                break;
            case '\f':
                escaped << "\\f";
                break;
            case '\n':
                escaped << "\\n";
                break;
            case '\r':
                escaped << "\\r";
                break;
            case '\t':
                escaped << "\\t";
                break;
            default:
                if (static_cast<unsigned char>(ch) < 0x20) {
                    escaped << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                            << static_cast<int>(static_cast<unsigned char>(ch))
                            << std::dec << std::setfill(' ');
                } else {
                    escaped << ch;
                }
                break;
        }
    }
    return escaped.str();
}

std::string simulation_run_to_json(const SimulationRunResult& result) {
    std::ostringstream os;
    os << "{\n";
    os << "  \"mode\": \"" << escape_json_string(result.mode) << "\",\n";
    os << "  \"scheduler\": \"" << escape_json_string(result.scheduler) << "\",\n";
    os << "  \"config\": {\n";
    os << "    \"sms\": " << result.sms << ",\n";
    os << "    \"warps\": " << result.warps << ",\n";
    os << "    \"instructions_per_warp\": " << result.instructions_per_warp << ",\n";
    os << "    \"memory_latency\": " << result.memory_latency << "\n";
    os << "  },\n";
    os << "  \"total_cycles\": " << result.metrics.total_cycles << ",\n";
    os << "  \"final_metrics\": ";
    append_metrics_json(os, result.metrics, 2);
    os << ",\n";
    os << "  \"timeline\": [\n";

    for (std::size_t cycle_index = 0; cycle_index < result.timeline.size(); ++cycle_index) {
        const CycleEvent& cycle = result.timeline[cycle_index];
        os << "    {\n";
        os << "      \"cycle\": " << cycle.cycle << ",\n";
        os << "      \"sms\": [\n";
        for (std::size_t sm_index = 0; sm_index < cycle.sms.size(); ++sm_index) {
            const SmEvent& event = cycle.sms[sm_index];
            os << "        {\n";
            os << "          \"sm_id\": " << event.sm_id << ",\n";
            os << "          \"warp_id\": " << event.warp_id << ",\n";
            os << "          \"instruction\": \"" << escape_json_string(event.instruction)
               << "\",\n";
            os << "          \"resulting_state\": \""
               << escape_json_string(event.resulting_state) << "\",\n";
            os << "          \"idle\": " << (event.idle ? "true" : "false") << '\n';
            os << "        }" << (sm_index + 1 == cycle.sms.size() ? "" : ",") << '\n';
        }
        os << "      ],\n";
        os << "      \"memory_stalls\": [\n";
        for (std::size_t stall_index = 0; stall_index < cycle.memory_stalls.size();
             ++stall_index) {
            const StallEvent& stall = cycle.memory_stalls[stall_index];
            os << "        {\"warp_id\": " << stall.warp_id
               << ", \"before\": " << stall.before
               << ", \"after\": " << stall.after << "}"
               << (stall_index + 1 == cycle.memory_stalls.size() ? "" : ",") << '\n';
        }
        os << "      ],\n";
        os << "      \"completed_warps\": ";
        append_string_array(os, cycle.completed_warps, 6);
        os << ",\n";
        os << "      \"warp_states\": [\n";
        for (std::size_t warp_index = 0; warp_index < cycle.warp_states.size();
             ++warp_index) {
            const WarpStateSnapshot& snapshot = cycle.warp_states[warp_index];
            os << "        {\"warp_id\": " << snapshot.warp_id
               << ", \"program_counter\": " << snapshot.program_counter
               << ", \"state\": \"" << escape_json_string(snapshot.state)
               << "\", \"stall_cycles_remaining\": "
               << snapshot.stall_cycles_remaining << "}"
               << (warp_index + 1 == cycle.warp_states.size() ? "" : ",") << '\n';
        }
        os << "      ]\n";
        os << "    }" << (cycle_index + 1 == result.timeline.size() ? "" : ",") << '\n';
    }

    os << "  ]\n";
    os << "}\n";
    return os.str();
}

std::string benchmark_to_json(const BenchmarkConfig& config,
                              const std::vector<BenchmarkResult>& results) {
    std::ostringstream os;
    os << "{\n";
    os << "  \"mode\": \"benchmark\",\n";
    os << "  \"workload\": {\n";
    os << "    \"warps\": " << config.workload.numWarps << ",\n";
    os << "    \"instructions_per_warp\": " << config.workload.instructionsPerWarp << ",\n";
    os << "    \"memory_probability\": " << config.workload.memoryInstructionProbability
       << ",\n";
    os << "    \"memory_latency\": " << config.workload.memoryLatency << ",\n";
    os << "    \"sms\": " << config.smCount << ",\n";
    os << "    \"seed\": " << config.workload.randomSeed << '\n';
    os << "  },\n";
    os << "  \"results\": [\n";

    for (std::size_t i = 0; i < results.size(); ++i) {
        os << "    {\n";
        os << "      \"scheduler\": \"" << escape_json_string(results[i].schedulerName)
           << "\",\n";
        os << "      \"metrics\": ";
        append_metrics_json(os, results[i].metrics, 6);
        os << '\n';
        os << "    }" << (i + 1 == results.size() ? "" : ",") << '\n';
    }

    os << "  ]\n";
    os << "}\n";
    return os.str();
}

void write_json_file(const std::string& path, const std::string& json) {
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("Could not open JSON export path: " + path);
    }
    out << json;
}

} // namespace gpuvision
