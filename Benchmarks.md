# Benchmark: add numbers concurrently
We have 6 benchmarks with different grain sizes; each was run 5 times on my matebook laptop.

### benchmark1 * 5
grain size=1000, workload size=10000000
c++ std::accumulate sum=49998576, elapsed=4ms
c++ std::async sum=49998576, elapsed=10817ms
tbb::parallel_reduce sum=49998576, elapsed=8ms
cfsched::parallel_sum=49998576, elapsed=3ms

grain size=1000, workload size=10000000
c++ std::accumulate sum=50010936, elapsed=4ms
c++ std::async sum=50010936, elapsed=10764ms
tbb::parallel_reduce sum=50010936, elapsed=8ms
cfsched::parallel_sum=50010936, elapsed=4ms

grain size=1000, workload size=10000000
c++ std::accumulate sum=50004007, elapsed=4ms
c++ std::async sum=50004007, elapsed=11160ms
tbb::parallel_reduce sum=50004007, elapsed=7ms
cfsched::parallel_sum=50004007, elapsed=4ms

grain size=1000, workload size=10000000
c++ std::accumulate sum=49983149, elapsed=4ms
c++ std::async sum=49983149, elapsed=9256ms
tbb::parallel_reduce sum=49983149, elapsed=7ms
cfsched::parallel_sum=49983149, elapsed=3ms

grain size=1000, workload size=10000000
c++ std::accumulate sum=50003155, elapsed=4ms
c++ std::async sum=50003155, elapsed=10366ms
tbb::parallel_reduce sum=50003155, elapsed=8ms
cfsched::parallel_sum=50003155, elapsed=3ms

### benchmark2 * 5
grain size=2000, workload size=10000000
c++ std::accumulate sum=49997051, elapsed=4ms
c++ std::async sum=49997051, elapsed=3362ms
tbb::parallel_reduce sum=49997051, elapsed=8ms
cfsched::parallel_sum=49997051, elapsed=2ms

grain size=2000, workload size=10000000
c++ std::accumulate sum=50000668, elapsed=4ms
c++ std::async sum=50000668, elapsed=2291ms
tbb::parallel_reduce sum=50000668, elapsed=9ms
cfsched::parallel_sum=50000668, elapsed=2ms

grain size=2000, workload size=10000000
c++ std::accumulate sum=50004384, elapsed=4ms
c++ std::async sum=50004384, elapsed=2188ms
tbb::parallel_reduce sum=50004384, elapsed=8ms
cfsched::parallel_sum=50004384, elapsed=2ms

grain size=2000, workload size=10000000
c++ std::accumulate sum=49983757, elapsed=4ms
c++ std::async sum=49983757, elapsed=1990ms
tbb::parallel_reduce sum=49983757, elapsed=8ms
cfsched::parallel_sum=49983757, elapsed=3ms

grain size=2000, workload size=10000000
c++ std::accumulate sum=49999264, elapsed=4ms
c++ std::async sum=49999264, elapsed=2181ms
tbb::parallel_reduce sum=49999264, elapsed=8ms
cfsched::parallel_sum=49999264, elapsed=2ms

### benchmark3 * 5
grain size=5000, workload size=10000000
c++ std::accumulate sum=49997541, elapsed=4ms
c++ std::async sum=49997541, elapsed=225ms
tbb::parallel_reduce sum=49997541, elapsed=9ms
cfsched::parallel_sum=49997541, elapsed=3ms

grain size=5000, workload size=10000000
c++ std::accumulate sum=49984396, elapsed=4ms
c++ std::async sum=49984396, elapsed=238ms
tbb::parallel_reduce sum=49984396, elapsed=8ms
cfsched::parallel_sum=49984396, elapsed=6ms

grain size=5000, workload size=10000000
c++ std::accumulate sum=49973660, elapsed=4ms
c++ std::async sum=49973660, elapsed=205ms
tbb::parallel_reduce sum=49973660, elapsed=8ms
cfsched::parallel_sum=49973660, elapsed=3ms

grain size=5000, workload size=10000000
c++ std::accumulate sum=49991342, elapsed=4ms
c++ std::async sum=49991342, elapsed=238ms
tbb::parallel_reduce sum=49991342, elapsed=8ms
cfsched::parallel_sum=49991342, elapsed=2ms

grain size=5000, workload size=10000000
c++ std::accumulate sum=49980386, elapsed=4ms
c++ std::async sum=49980386, elapsed=236ms
tbb::parallel_reduce sum=49980386, elapsed=8ms
cfsched::parallel_sum=49980386, elapsed=2ms

### benchmark4 * 5
grain size=10000, workload size=10000000
c++ std::accumulate sum=49998440, elapsed=4ms
c++ std::async sum=49998440, elapsed=115ms
tbb::parallel_reduce sum=49998440, elapsed=10ms
cfsched::parallel_sum=49998440, elapsed=2ms

grain size=10000, workload size=10000000
c++ std::accumulate sum=50001828, elapsed=4ms
c++ std::async sum=50001828, elapsed=117ms
tbb::parallel_reduce sum=50001828, elapsed=9ms
cfsched::parallel_sum=50001828, elapsed=1ms

grain size=10000, workload size=10000000
c++ std::accumulate sum=50005563, elapsed=4ms
c++ std::async sum=50005563, elapsed=115ms
tbb::parallel_reduce sum=50005563, elapsed=9ms
cfsched::parallel_sum=50005563, elapsed=1ms

grain size=10000, workload size=10000000
c++ std::accumulate sum=50000890, elapsed=4ms
c++ std::async sum=50000890, elapsed=111ms
tbb::parallel_reduce sum=50000890, elapsed=9ms
cfsched::parallel_sum=50000890, elapsed=2ms

grain size=10000, workload size=10000000
c++ std::accumulate sum=49986962, elapsed=4ms
c++ std::async sum=49986962, elapsed=115ms
tbb::parallel_reduce sum=49986962, elapsed=9ms
cfsched::parallel_sum=49986962, elapsed=1ms

### benchmark5 * 5
grain size=50000, workload size=10000000
c++ std::accumulate sum=49975222, elapsed=10ms
c++ std::async sum=49975222, elapsed=49ms
tbb::parallel_reduce sum=49975222, elapsed=17ms
cfsched::parallel_sum=49975222, elapsed=2ms

grain size=50000, workload size=10000000
c++ std::accumulate sum=50003865, elapsed=10ms
c++ std::async sum=50003865, elapsed=42ms
tbb::parallel_reduce sum=50003865, elapsed=13ms
cfsched::parallel_sum=50003865, elapsed=3ms

grain size=50000, workload size=10000000
c++ std::accumulate sum=49997861, elapsed=10ms
c++ std::async sum=49997861, elapsed=48ms
tbb::parallel_reduce sum=49997861, elapsed=5ms
cfsched::parallel_sum=49997861, elapsed=2ms

grain size=50000, workload size=10000000
c++ std::accumulate sum=50007132, elapsed=10ms
c++ std::async sum=50007132, elapsed=44ms
tbb::parallel_reduce sum=50007132, elapsed=12ms
cfsched::parallel_sum=50007132, elapsed=4ms

grain size=50000, workload size=10000000
c++ std::accumulate sum=49993716, elapsed=8ms
c++ std::async sum=49993716, elapsed=45ms
tbb::parallel_reduce sum=49993716, elapsed=7ms
cfsched::parallel_sum=49993716, elapsed=3ms





