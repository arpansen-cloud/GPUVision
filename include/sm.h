#ifndef GPUVISION_SM_H
#define GPUVISION_SM_H

#include <memory>
#include <ostream>
#include <vector>

#include "metrics.h"
#include "json_export.h"
#include "scheduler.h"
#include "warp.h"

namespace gpuvision {

class SM {
public:
    SM(int id, std::unique_ptr<Scheduler> scheduler);

    int id() const;

    // Issues at most one warp instruction this cycle.
    bool issue_cycle(std::vector<Warp>& warps,
                     int memory_latency,
                     Metrics& metrics,
                     std::ostream* log,
                     SmEvent* event = nullptr);

private:
    int id_;
    std::unique_ptr<Scheduler> scheduler_;
};

} // namespace gpuvision

#endif // GPUVISION_SM_H
