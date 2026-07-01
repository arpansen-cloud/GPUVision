#include "sm.h"

#include <ostream>
#include <utility>

namespace gpuvision {

SM::SM(int id, std::unique_ptr<Scheduler> scheduler)
    : id_(id), scheduler_(std::move(scheduler)) {}

int SM::id() const {
    return id_;
}

bool SM::issue_cycle(std::vector<Warp>& warps,
                     int memory_latency,
                     Metrics& metrics,
                     std::ostream* log) {
    ++metrics.total_sm_cycles;

    const int warp_index = scheduler_->select_warp(warps);
    if (warp_index < 0) {
        ++metrics.idle_cycles;
        if (log != nullptr) {
            *log << "  SM " << id_ << ": idle\n";
        }
        return false;
    }

    Warp& warp = warps[static_cast<std::size_t>(warp_index)];
    warp.mark_running();
    const bool memory_instruction = warp.issue_next_instruction(memory_latency);
    if (warp.state() == WarpState::READY) {
        warp.mark_running();
    }
    ++metrics.instructions_completed;
    ++metrics.non_idle_sm_cycles;
    ++metrics.active_sm_cycles;

    if (memory_instruction) {
        ++metrics.memory_stall_count;
    }

    if (log != nullptr) {
        *log << "  SM " << id_ << ": issued warp " << warp.id()
             << (memory_instruction ? " MEMORY" : " COMPUTE")
             << ", pc=" << warp.program_counter()
             << ", state=" << to_string(warp.state()) << '\n';
    }

    return true;
}

} // namespace gpuvision
