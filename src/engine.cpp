#include "engine.h"

#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace gpuvision {

Engine::Engine(EngineConfig config, std::vector<Warp> warps)
    : config_(config), warps_(std::move(warps)) {
    if (config_.sm_count <= 0) {
        throw std::invalid_argument("SM count must be positive");
    }
    if (config_.memory_latency < 0) {
        throw std::invalid_argument("Memory latency cannot be negative");
    }

    sms_.reserve(static_cast<std::size_t>(config_.sm_count));
    for (int id = 0; id < config_.sm_count; ++id) {
        sms_.emplace_back(id, create_scheduler(config_.scheduler_type));
    }
    update_completed_warps_metric();
}

const Metrics& Engine::run(std::ostream* log) {
    while (!all_warps_completed()) {
        if (config_.enable_cycle_logs && log != nullptr) {
            *log << "Cycle " << metrics_.total_cycles << '\n';
        }

        tick_stalled_warps(config_.enable_cycle_logs ? log : nullptr);

        for (SM& sm : sms_) {
            sm.issue_cycle(warps_, config_.memory_latency, metrics_,
                           config_.enable_cycle_logs ? log : nullptr);
        }

        for (Warp& warp : warps_) {
            warp.mark_ready();
        }

        ++metrics_.total_cycles;
        update_completed_warps_metric();
    }

    return metrics_;
}

const Metrics& Engine::metrics() const {
    return metrics_;
}

const std::vector<Warp>& Engine::warps() const {
    return warps_;
}

bool Engine::all_warps_completed() const {
    return std::all_of(warps_.begin(), warps_.end(), [](const Warp& warp) {
        return warp.is_completed();
    });
}

void Engine::tick_stalled_warps(std::ostream* log) {
    for (Warp& warp : warps_) {
        const int before = warp.stall_cycles_remaining();
        warp.tick_stall();
        if (log != nullptr && before > 0) {
            *log << "  Warp " << warp.id() << " stall " << before << " -> "
                 << warp.stall_cycles_remaining() << '\n';
        }
    }
}

void Engine::update_completed_warps_metric() {
    metrics_.completed_warps = static_cast<int>(std::count_if(
        warps_.begin(), warps_.end(), [](const Warp& warp) { return warp.is_completed(); }));
}

} // namespace gpuvision
