#pragma once

#include <future>
#include <algorithm>

template <int GrainSize, typename RandomIt>
int std_parallel_sum(RandomIt beg, RandomIt end)
{
    auto len = end - beg;
    if (len < GrainSize) return std::accumulate(beg, end, 0);
    RandomIt mid = beg + len/2;
    auto handle = std::async(std::launch::async, std_parallel_sum<GrainSize,RandomIt>, mid, end);
    int sum = std_parallel_sum<GrainSize>(beg, mid);
    return sum + handle.get();
}