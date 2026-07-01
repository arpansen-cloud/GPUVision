#ifndef GPUVISION_SCHEDULER_H
#define GPUVISION_SCHEDULER_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "warp.h"

namespace gpuvision {

enum class SchedulerType {
    ROUND_ROBIN,
    OLDEST_FIRST,
    GREEDY_THEN_OLDEST
};

class Scheduler {
public:
    virtual ~Scheduler() = default;
    virtual int select_warp(const std::vector<Warp>& warps) = 0;
    virtual std::string name() const = 0;
};

class RoundRobinScheduler final : public Scheduler {
public:
    int select_warp(const std::vector<Warp>& warps) override;
    std::string name() const override;

private:
    std::size_t next_index_ = 0;
};

class OldestFirstScheduler final : public Scheduler {
public:
    int select_warp(const std::vector<Warp>& warps) override;
    std::string name() const override;
};

class GreedyThenOldestScheduler final : public Scheduler {
public:
    int select_warp(const std::vector<Warp>& warps) override;
    std::string name() const override;

private:
    int last_warp_id_ = -1;
};

std::unique_ptr<Scheduler> create_scheduler(SchedulerType type);

} // namespace gpuvision

#endif // GPUVISION_SCHEDULER_H
