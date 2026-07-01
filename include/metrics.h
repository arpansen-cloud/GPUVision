#ifndef GPUVISION_METRICS_H
#define GPUVISION_METRICS_H

#include <iosfwd>

namespace gpuvision {

struct Metrics {
    int total_cycles = 0;
    int instructions_completed = 0;
    int idle_cycles = 0;
    int memory_stall_count = 0;
    int completed_warps = 0;
    int total_sm_cycles = 0;
    int non_idle_sm_cycles = 0;
    int active_sm_cycles = 0;
    double average_warp_completion_time = 0.0;
    int max_warp_completion_time = 0;
    int min_warp_completion_time = 0;

    double ipc() const;
    double sm_utilization_percent() const;
};

std::ostream& operator<<(std::ostream& os, const Metrics& metrics);

} // namespace gpuvision

#endif // GPUVISION_METRICS_H
