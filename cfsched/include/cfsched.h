#pragma once

#include "task_logical.h"
#include "parallel_sum.h"

namespace cfsched{

/*==================================================
 Some API functions of cfsched library

 init() initialises and starts the task pool.
 
 sched() creates and schedules a root task in the pool.  

 parallel_sum() calculates sum of integers in parallel.

===================================================*/

static inline void init(){
    Pool::instance().start();
}

template < class T, class... Args >  
T* sched(Args&&... args)
{
    Pool::instance().emplaceRoot<T>(std::forward<Args>(args)...);
}

template <int GrainSize>
int parallel_sum(const int* arr, size_t n){
    volatile int sum;
	ParallelSum<GrainSize>* task=Pool::instance().emplaceRoot<ParallelSum<GrainSize>>(arr, arr+n, &sum);
    task->externalSync(); 
    return sum;
}

}