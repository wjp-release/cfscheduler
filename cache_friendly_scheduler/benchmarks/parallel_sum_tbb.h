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

template <int GrainSize>
class TBBSumTask: public task {
public:
    const int* beg;
    const int* end;
    int* sum;
    TBBSumTask(const int* b, const int* e, int* s):beg(b), end(e), sum(s){}
    task* execute() {  
        auto len = end - beg;
        if (len < GrainSize){
            *sum = std::accumulate(beg, end, 0);
            return 0;
        } 
        const int* mid = beg + len/2;
        int x,y;
        TBBSumTask<GrainSize>& a = *new( allocate_child() ) TBBSumTask<GrainSize>(mid,end,&x);
        TBBSumTask<GrainSize>& b = *new( allocate_child() ) TBBSumTask<GrainSize>(beg,mid,&y);
        set_ref_count(3); // 2 children + self
        spawn( b ); // start b running.
        spawn_and_wait_for_all(a); // start a & wait for all
        *sum = x+y;
        return 0;
    }
};

template <int GrainSize>
int tbb_fjtask_sum(const int* arr, size_t n){
    int sum;
    TBBSumTask<GrainSize>& a = *new(task::allocate_root()) TBBSumTask<GrainSize>(arr, arr+n, &sum);
    task::spawn_root_and_wait(a);
    return sum;
}


