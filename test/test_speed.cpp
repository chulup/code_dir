#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <vector>

#include "src/codename.h"

#define SAMPLES 1'000'000
static const uint64_t random_sum = 1073890749828105;

TEST(random, generate) {
    uint64_t sum = 0;
    std::srand((int)SAMPLES);

    for (size_t count = 0; count < SAMPLES; ++count) {
        sum += std::rand();
    }

    EXPECT_EQ(sum, random_sum);
}

TEST(random, vector_generate) {
    std::srand((int)SAMPLES);

    std::vector<int> vec;
    vec.resize(SAMPLES);

    for (size_t count = 0; count < SAMPLES; ++count) {
        vec[count] = std::rand();
    }

    EXPECT_EQ(random_sum, std::accumulate(vec.begin(), vec.end(), uint64_t(0)));
}

