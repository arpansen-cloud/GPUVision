#include "metrics.h"

#include <iomanip>
#include <ostream>

namespace gpuvision {

double Metrics::ipc() const {
    if (total_cycles == 0) {
        return 0.0;
    }
    return static_cast<double>(instructions_completed) / static_cast<double>(total_cycles);
}

double Metrics::sm_utilization_percent() const {
    if (total_sm_cycles == 0) {
        return 0.0;
    }
    return 100.0 * static_cast<double>(active_sm_cycles) / static_cast<double>(total_sm_cycles);
}

std::ostream& operator<<(std::ostream& os, const Metrics& metrics) {
    os << "Final Metrics\n";
    os << "  Total cycles: " << metrics.total_cycles << '\n';
    os << "  Instructions completed: " << metrics.instructions_completed << '\n';
    os << "  IPC: " << std::fixed << std::setprecision(2) << metrics.ipc() << '\n';
    os << "  SM utilization: " << std::fixed << std::setprecision(2)
       << metrics.sm_utilization_percent() << "%\n";
    os << "  Idle cycles: " << metrics.idle_cycles << '\n';
    os << "  Memory stall count: " << metrics.memory_stall_count << '\n';
    os << "  Completed warps: " << metrics.completed_warps << '\n';
    return os;
}

} // namespace gpuvision
