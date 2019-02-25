# Benchmark: add numbers concurrently
We have 5 configurations; each has different grain size; each was run 5 times on my laptop(Matebook x pro, Win10 Linux Subsystem, 8*Intel i5-8250U CPU @ 1.60GHz).

### What does grain size mean?
When the number of integers a task needs to add up is smaller than grain size, it will stop spawning child tasks and instead add them up sequentially. 
Larger grain size means fewer child tasks. 

### What is std::accumulate, tbb::parallel_reduce and std::async?

std::accumulate adds up integers sequentially.

std::async uses C++11's user-level task pool to calculate sum concurrently. The algorithm implementation is copied from cppreference example: https://en.cppreference.com/w/cpp/thread/async.

```cpp
template <typename RandomIt>
int parallel_sum(RandomIt beg, RandomIt end)
{
    auto len = end - beg;
    if (len < 1000)
        return std::accumulate(beg, end, 0);
 
    RandomIt mid = beg + len/2;
    auto handle = std::async(std::launch::async,
                             parallel_sum<RandomIt>, mid, end);
    int sum = parallel_sum(beg, mid);
    return sum + handle.get();
}
```

Intel tbb is probably the best C++ concurrent library. By defining a joinable structure, you can use tbb::parallel_reduce to implement the exact algorithm cited above.

cfsched::parallel_sum implements this algorithm using libcfsched. 

### benchmark1 * 5
grain size=1000, workload size=10000000
c++ std::accumulate sum=49998576, elapsed=4 ms
c++ std::async sum=49998576, elapsed=10817 ms
tbb::parallel_reduce sum=49998576, elapsed=8 ms
cfsched::parallel_sum=49998576, elapsed=3 ms

grain size=1000, workload size=10000000
c++ std::accumulate sum=50010936, elapsed=4 ms
c++ std::async sum=50010936, elapsed=10764 ms
tbb::parallel_reduce sum=50010936, elapsed=8 ms
cfsched::parallel_sum=50010936, elapsed=4 ms

grain size=1000, workload size=10000000
c++ std::accumulate sum=50004007, elapsed=4 ms
c++ std::async sum=50004007, elapsed=11160 ms
tbb::parallel_reduce sum=50004007, elapsed=7 ms
cfsched::parallel_sum=50004007, elapsed=4 ms

grain size=1000, workload size=10000000
c++ std::accumulate sum=49983149, elapsed=4 ms
c++ std::async sum=49983149, elapsed=9256 ms
tbb::parallel_reduce sum=49983149, elapsed=7 ms
cfsched::parallel_sum=49983149, elapsed=3 ms

grain size=1000, workload size=10000000
c++ std::accumulate sum=50003155, elapsed=4 ms
c++ std::async sum=50003155, elapsed=10366 ms
tbb::parallel_reduce sum=50003155, elapsed=8 ms
cfsched::parallel_sum=50003155, elapsed=3 ms

### benchmark2 * 5
grain size=2000, workload size=10000000
c++ std::accumulate sum=49997051, elapsed=4 ms
c++ std::async sum=49997051, elapsed=3362 ms
tbb::parallel_reduce sum=49997051, elapsed=8 ms
cfsched::parallel_sum=49997051, elapsed=2 ms

grain size=2000, workload size=10000000
c++ std::accumulate sum=50000668, elapsed=4 ms
c++ std::async sum=50000668, elapsed=2291 ms
tbb::parallel_reduce sum=50000668, elapsed=9 ms
cfsched::parallel_sum=50000668, elapsed=2 ms

grain size=2000, workload size=10000000
c++ std::accumulate sum=50004384, elapsed=4 ms
c++ std::async sum=50004384, elapsed=2188 ms
tbb::parallel_reduce sum=50004384, elapsed=8 ms
cfsched::parallel_sum=50004384, elapsed=2 ms

grain size=2000, workload size=10000000
c++ std::accumulate sum=49983757, elapsed=4 ms
c++ std::async sum=49983757, elapsed=1990 ms
tbb::parallel_reduce sum=49983757, elapsed=8 ms
cfsched::parallel_sum=49983757, elapsed=3 ms

grain size=2000, workload size=10000000
c++ std::accumulate sum=49999264, elapsed=4 ms
c++ std::async sum=49999264, elapsed=2181 ms
tbb::parallel_reduce sum=49999264, elapsed=8 ms
cfsched::parallel_sum=49999264, elapsed=2 ms

### benchmark3 * 5
grain size=5000, workload size=10000000
c++ std::accumulate sum=49997541, elapsed=4 ms
c++ std::async sum=49997541, elapsed=225 ms
tbb::parallel_reduce sum=49997541, elapsed=9 ms
cfsched::parallel_sum=49997541, elapsed=3 ms

grain size=5000, workload size=10000000
c++ std::accumulate sum=49984396, elapsed=4 ms
c++ std::async sum=49984396, elapsed=238 ms
tbb::parallel_reduce sum=49984396, elapsed=8 ms
cfsched::parallel_sum=49984396, elapsed=6 ms

grain size=5000, workload size=10000000
c++ std::accumulate sum=49973660, elapsed=4 ms
c++ std::async sum=49973660, elapsed=205 ms
tbb::parallel_reduce sum=49973660, elapsed=8 ms
cfsched::parallel_sum=49973660, elapsed=3 ms

grain size=5000, workload size=10000000
c++ std::accumulate sum=49991342, elapsed=4 ms
c++ std::async sum=49991342, elapsed=238 ms
tbb::parallel_reduce sum=49991342, elapsed=8 ms
cfsched::parallel_sum=49991342, elapsed=2 ms

grain size=5000, workload size=10000000
c++ std::accumulate sum=49980386, elapsed=4 ms
c++ std::async sum=49980386, elapsed=236 ms
tbb::parallel_reduce sum=49980386, elapsed=8 ms
cfsched::parallel_sum=49980386, elapsed=2 ms

### benchmark4 * 5
grain size=10000, workload size=10000000
c++ std::accumulate sum=49998440, elapsed=4 ms
c++ std::async sum=49998440, elapsed=115 ms
tbb::parallel_reduce sum=49998440, elapsed=10 ms
cfsched::parallel_sum=49998440, elapsed=2 ms

grain size=10000, workload size=10000000
c++ std::accumulate sum=50001828, elapsed=4 ms
c++ std::async sum=50001828, elapsed=117 ms
tbb::parallel_reduce sum=50001828, elapsed=9 ms
cfsched::parallel_sum=50001828, elapsed=1 ms

grain size=10000, workload size=10000000
c++ std::accumulate sum=50005563, elapsed=4 ms
c++ std::async sum=50005563, elapsed=115 ms
tbb::parallel_reduce sum=50005563, elapsed=9 ms
cfsched::parallel_sum=50005563, elapsed=1 ms

grain size=10000, workload size=10000000
c++ std::accumulate sum=50000890, elapsed=4 ms
c++ std::async sum=50000890, elapsed=111 ms
tbb::parallel_reduce sum=50000890, elapsed=9 ms
cfsched::parallel_sum=50000890, elapsed=2 ms

grain size=10000, workload size=10000000
c++ std::accumulate sum=49986962, elapsed=4 ms
c++ std::async sum=49986962, elapsed=115 ms
tbb::parallel_reduce sum=49986962, elapsed=9 ms
cfsched::parallel_sum=49986962, elapsed=1 ms

### benchmark5 * 5
grain size=50000, workload size=10000000
c++ std::accumulate sum=49975222, elapsed=10 ms
c++ std::async sum=49975222, elapsed=49 ms
tbb::parallel_reduce sum=49975222, elapsed=17 ms
cfsched::parallel_sum=49975222, elapsed=2 ms

grain size=50000, workload size=10000000
c++ std::accumulate sum=50003865, elapsed=10 ms
c++ std::async sum=50003865, elapsed=42 ms
tbb::parallel_reduce sum=50003865, elapsed=13 ms
cfsched::parallel_sum=50003865, elapsed=3 ms

grain size=50000, workload size=10000000
c++ std::accumulate sum=49997861, elapsed=10 ms
c++ std::async sum=49997861, elapsed=48 ms
tbb::parallel_reduce sum=49997861, elapsed=5 ms
cfsched::parallel_sum=49997861, elapsed=2 ms

grain size=50000, workload size=10000000
c++ std::accumulate sum=50007132, elapsed=10 ms
c++ std::async sum=50007132, elapsed=44 ms
tbb::parallel_reduce sum=50007132, elapsed=12 ms
cfsched::parallel_sum=50007132, elapsed=4 ms

grain size=50000, workload size=10000000
c++ std::accumulate sum=49993716, elapsed=8 ms
c++ std::async sum=49993716, elapsed=45 ms
tbb::parallel_reduce sum=49993716, elapsed=7 ms
cfsched::parallel_sum=49993716, elapsed=3 ms





