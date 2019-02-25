#pragma once

#include "utils.h"
#include "task_logical.h"
#include <numeric>
#include <atomic>
#include <cassert>
#include "options.h"
namespace cfsched{

template <int GrainSize>
class ParallelSum: public Task {
public:
    const int* beg;
    const int* end;
    int* sum;
    ParallelSum(const int* b, const int* e, int* s):beg(b), end(e), sum(s){}
    std::string stats() override{
		return std::to_string((uint64_t)(end-beg));
	}
	void compute() override{
    #ifdef EnableDebugging  
        auto t=FixSizedTask::getFixSizedTaskPointer(this);
        t->assertBeforeSpawn(); 
    #endif
        auto len = end - beg;
        if (len < GrainSize){
            *sum = std::accumulate(beg, end, 0);
    #ifdef EnableDebugging  
            assert(t->meta.pendingcnt.load()==0);
            assert(*sum>0);
    #endif
            return;
        } 
        const int* mid = beg + len/2;
        int x=-123;
        int y=-123;
    #ifdef EnableDebugging  
        assert(t->taskPointer()==this);
    #endif
        spawnPrivate<ParallelSum>(mid,end,&x);
		spawn<ParallelSum>(beg,mid,&y);
        localSync();
    #ifdef EnableDebugging    
        t->assertAfterSync();
        assert(t->meta.synced==true);
    #endif
        *sum=x+y;    
    #ifdef EnableDebugging    
        if(x<0) println("x="+std::to_string(x));
        if(y<0) println("y="+std::to_string(y));
        if(*sum<0) println("sum="+std::to_string(*sum));
        //assert(x>0);
        //assert(y>0);
        //assert(*sum>0);
        //println(">> sum="+std::to_string(x)+"+"+std::to_string(y));
    #endif
    }
};


}