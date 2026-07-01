#include "engine.h"

#include <algorithm>
#include <limits>
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
    warp_completion_times_.assign(warps_.size(), -1);
    record_newly_completed_warps(0);
    update_completed_warps_metric();
}

const Metrics& Engine::run(std::ostream* log) {
    while (!all_warps_completed()) {
        CycleEvent cycle_event;
        cycle_event.cycle = metrics_.total_cycles;

        if (config_.enable_cycle_logs && log != nullptr) {
            *log << "Cycle " << metrics_.total_cycles << '\n';
        }

        tick_stalled_warps(config_.enable_cycle_logs ? log : nullptr,
                           config_.capture_timeline ? &cycle_event : nullptr);

        for (SM& sm : sms_) {
            SmEvent sm_event;
            sm.issue_cycle(warps_, config_.memory_latency, metrics_,
                           config_.enable_cycle_logs ? log : nullptr,
                           config_.capture_timeline ? &sm_event : nullptr);
            if (config_.capture_timeline) {
                cycle_event.sms.push_back(sm_event);
            }
        }

        for (Warp& warp : warps_) {
            warp.mark_ready();
        }

        ++metrics_.total_cycles;
        record_newly_completed_warps(metrics_.total_cycles);
        if (config_.capture_timeline) {
            for (std::size_t i = 0; i < warps_.size(); ++i) {
                if (warp_completion_times_[i] == metrics_.total_cycles) {
                    cycle_event.completed_warps.push_back(warps_[i].id());
                }
                cycle_event.warp_states.push_back(
                    {warps_[i].id(),
                     warps_[i].program_counter(),
                     to_string(warps_[i].state()),
                     warps_[i].stall_cycles_remaining()});
            }
            timeline_.push_back(cycle_event);
        }
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

const std::vector<CycleEvent>& Engine::timeline() const {
    return timeline_;
}

bool Engine::all_warps_completed() const {
    return std::all_of(warps_.begin(), warps_.end(), [](const Warp& warp) {
        return warp.is_completed();
    });
}

void Engine::tick_stalled_warps(std::ostream* log, CycleEvent* cycle_event) {
    for (Warp& warp : warps_) {
        const int before = warp.stall_cycles_remaining();
        warp.tick_stall();
        if (log != nullptr && before > 0) {
            *log << "  Warp " << warp.id() << " stall " << before << " -> "
                 << warp.stall_cycles_remaining() << '\n';
        }
        if (cycle_event != nullptr && before > 0) {
            cycle_event->memory_stalls.push_back(
                {warp.id(), before, warp.stall_cycles_remaining()});
        }
    }
}

void Engine::update_completed_warps_metric() {
    metrics_.completed_warps = static_cast<int>(std::count_if(
        warps_.begin(), warps_.end(), [](const Warp& warp) { return warp.is_completed(); }));
    update_completion_time_metrics();
}

void Engine::record_newly_completed_warps(int completion_cycle) {
    for (std::size_t i = 0; i < warps_.size(); ++i) {
        if (warps_[i].is_completed() && warp_completion_times_[i] < 0) {
            warp_completion_times_[i] = completion_cycle;
        }
    }
}

void Engine::update_completion_time_metrics() {
    int count = 0;
    int sum = 0;
    int min_time = std::numeric_limits<int>::max();
    int max_time = 0;

    for (int completion_time : warp_completion_times_) {
        if (completion_time < 0) {
            continue;
        }
        ++count;
        sum += completion_time;
        min_time = std::min(min_time, completion_time);
        max_time = std::max(max_time, completion_time);
    }

    if (count == 0) {
        metrics_.average_warp_completion_time = 0.0;
        metrics_.min_warp_completion_time = 0;
        metrics_.max_warp_completion_time = 0;
        return;
    }

    metrics_.average_warp_completion_time = static_cast<double>(sum) / static_cast<double>(count);
    metrics_.min_warp_completion_time = min_time;
    metrics_.max_warp_completion_time = max_time;
}

} // namespace gpuvision
