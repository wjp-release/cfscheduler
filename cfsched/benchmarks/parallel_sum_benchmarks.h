#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include "cfsched.h"
#include "parallel_sum_stdcpp.h"
#include "parallel_sum_tbb.h"

using namespace cfsched;

namespace parallel_sum_benchmarks{

template < int GrainSize, int WorkloadSize >
void time_measurement()
{
    std::cout<<"grain size="<<GrainSize<<", workload size="<<WorkloadSize<<"\n";
    std::vector<int> v(WorkloadSize);
    for(int i=0;i<WorkloadSize;i++){
        v[i]=randint<0,10>();
    }

    // std::accumulate sum
    time_point time_start=now();
    int sum_of_v=std::accumulate(v.begin(), v.end(), 0);
    int time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"c++ std::accumulate sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";
    
    // serial sum
    // time_start=now();
    // sum_of_v=0;
    // for(int i=0;i<WorkloadSize;i++){
    //     sum_of_v+=v[i];
    // }
    // time_elapsed=ms_elapsed_count(time_start);
    // std::cout<<"forloop sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";

    // std::async(fork/join) sum 
    if(GrainSize>=1000){
        time_start=now();
        sum_of_v=std_parallel_sum<GrainSize>(v.begin(), v.end());
        time_elapsed=ms_elapsed_count(time_start);
        std::cout<<"c++ std::async sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";
    }

    // tbb::parallel_reduce sum
    time_start=now();
    sum_of_v=tbb_parallel_sum<GrainSize>(v.data(), v.size());
    time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"tbb::parallel_reduce sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";

    // cfsched prepare the pool
    Pool::instance().start();
    // cfsched(fork/join) sum
    time_start=now();
    sum_of_v= cfsched::parallel_sum<GrainSize>(v.data(), v.size());
    time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"cfsched::parallel_sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";
}




}