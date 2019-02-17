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
		return std::to_string((uint64_t)(end-beg));
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
        if(x<0) println("x="+std::to_string(x));
        if(y<0) println("y="+std::to_string(y));
        if(*sum<0) println("sum="+std::to_string(*sum));
        assert(x>0);
        assert(y>0);
        assert(*sum>0);
        //奇怪的bug：这里不加sleep或println会导致release版死锁，导致debug版core dumped。sleep(1)有时好有时坏。。
        //println(">> sum="+std::to_string(x)+"+"+std::to_string(y));
    }
};


}