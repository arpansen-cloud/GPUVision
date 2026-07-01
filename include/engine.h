#ifndef GPUVISION_ENGINE_H
#define GPUVISION_ENGINE_H

#include <iosfwd>
#include <vector>

#include "json_export.h"
#include "metrics.h"
#include "scheduler.h"
#include "sm.h"
#include "warp.h"

namespace gpuvision {

struct EngineConfig {
    int sm_count = 1;
    int memory_latency = 4;
    SchedulerType scheduler_type = SchedulerType::ROUND_ROBIN;
    bool enable_cycle_logs = false;
    bool capture_timeline = false;
};

class Engine {
public:
    Engine(EngineConfig config, std::vector<Warp> warps);

    const Metrics& run(std::ostream* log = nullptr);
    const Metrics& metrics() const;
    const std::vector<Warp>& warps() const;
    const std::vector<CycleEvent>& timeline() const;

private:
    bool all_warps_completed() const;
    void tick_stalled_warps(std::ostream* log, CycleEvent* cycle_event);
    void update_completed_warps_metric();
    void record_newly_completed_warps(int completion_cycle);
    void update_completion_time_metrics();

    EngineConfig config_;
    std::vector<Warp> warps_;
    std::vector<SM> sms_;
    Metrics metrics_;
    std::vector<int> warp_completion_times_;
    std::vector<CycleEvent> timeline_;
};

} // namespace gpuvision

#endif // GPUVISION_ENGINE_H
