#include <gtest/gtest.h>

#include "workload.h"

TEST(WorkloadTests, SameSeedProducesSameWorkload) {
    const gpuvision::WorkloadConfig config{16, 20, 0.35, 12, 123};

    const std::vector<gpuvision::Warp> first = gpuvision::generate_workload(config);
    const std::vector<gpuvision::Warp> second = gpuvision::generate_workload(config);

    EXPECT_TRUE(gpuvision::workloads_equal(first, second));
}

TEST(WorkloadTests, DifferentSeedsProduceDifferentWorkloads) {
    const gpuvision::WorkloadConfig first_config{16, 20, 0.35, 12, 123};
    const gpuvision::WorkloadConfig second_config{16, 20, 0.35, 12, 456};

    const std::vector<gpuvision::Warp> first = gpuvision::generate_workload(first_config);
    const std::vector<gpuvision::Warp> second = gpuvision::generate_workload(second_config);

    EXPECT_FALSE(gpuvision::workloads_equal(first, second));
}
