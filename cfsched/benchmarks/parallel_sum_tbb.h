#pragma once

#include "tbb/parallel_reduce.h"
#include "tbb/blocked_range.h"
#include <algorithm>

using namespace tbb;

struct tbbsum{
    int value;
    tbbsum() :value(0){}
    tbbsum(tbbsum&s,split){value=0;}
    void operator()(const blocked_range<const int*>& range){
        int temp = value;
        for(const int* a=range.begin();a!=range.end();a++){
            temp+=*a;
        }
        value=temp;
    }
    void join(tbbsum&rhs){
        value+=rhs.value;
    }
};

template <int GrainSize>
int tbb_parallel_sum(const int* arr, size_t n){
    tbbsum total;
    parallel_reduce( blocked_range<const int*>(arr,arr+n,GrainSize), total);
    return total.value;
}

