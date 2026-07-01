#include "scheduler.h"

#include <algorithm>
#include <iterator>
#include <memory>

namespace gpuvision {

namespace {

int first_ready_index(const std::vector<Warp>& warps) {
    for (std::size_t i = 0; i < warps.size(); ++i) {
        if (warps[i].is_ready()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

} // namespace

int RoundRobinScheduler::select_warp(const std::vector<Warp>& warps) {
    if (warps.empty()) {
        return -1;
    }

    for (std::size_t checked = 0; checked < warps.size(); ++checked) {
        const std::size_t index = (next_index_ + checked) % warps.size();
        if (warps[index].is_ready()) {
            next_index_ = (index + 1) % warps.size();
            return static_cast<int>(index);
        }
    }

    return -1;
}

std::string RoundRobinScheduler::name() const {
    return "Round Robin";
}

int OldestFirstScheduler::select_warp(const std::vector<Warp>& warps) {
    return first_ready_index(warps);
}

std::string OldestFirstScheduler::name() const {
    return "Oldest First";
}

int GreedyThenOldestScheduler::select_warp(const std::vector<Warp>& warps) {
    const auto same_warp = std::find_if(warps.begin(), warps.end(), [this](const Warp& warp) {
        return warp.id() == last_warp_id_ && warp.is_ready();
    });

    if (same_warp != warps.end()) {
        return static_cast<int>(std::distance(warps.begin(), same_warp));
    }

    const int oldest_ready = first_ready_index(warps);
    if (oldest_ready >= 0) {
        last_warp_id_ = warps[static_cast<std::size_t>(oldest_ready)].id();
    }
    return oldest_ready;
}

std::string GreedyThenOldestScheduler::name() const {
    return "Greedy Then Oldest";
}

std::unique_ptr<Scheduler> create_scheduler(SchedulerType type) {
    switch (type) {
        case SchedulerType::ROUND_ROBIN:
            return std::make_unique<RoundRobinScheduler>();
        case SchedulerType::OLDEST_FIRST:
            return std::make_unique<OldestFirstScheduler>();
        case SchedulerType::GREEDY_THEN_OLDEST:
            return std::make_unique<GreedyThenOldestScheduler>();
    }
    return std::make_unique<RoundRobinScheduler>();
}

} // namespace gpuvision
