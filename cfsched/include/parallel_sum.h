#pragma once

#include "utils.h"
#include "task_logical.h"
#include <numeric>

namespace cfsched{

template <int GrainSize>
class ParallelSum: public Task {
public:
    const int* beg;
    const int* end;
    volatile int* sum;
    ParallelSum(const int* b, const int* e, volatile int* s):beg(b), end(e), sum(s){}
    std::string stats() override{
		return Task::stats()+"("+std::to_string((uint64_t)(end-beg))+")";
	}
	void compute() override{
        auto len = end - beg;
        if (len < GrainSize){
            *sum = std::accumulate(beg, end, 0);
            return;
        } 
        const int* mid = beg + len/2;
        volatile int x=-123;
        volatile int y=-123;
        spawn<ParallelSum>(mid,end,&x);
		spawnPrivate<ParallelSum>(beg,mid,&y);
        localSync();
        *sum=x+y;
        println(">> sum="+std::to_string(x)+"+"+std::to_string(y));
    }
};


}