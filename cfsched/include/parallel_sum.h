#pragma once

#include "utils.h"
#include "task_logical.h"
#include <numeric>
#include <atomic>
#include <cassert>

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
        auto t=FixSizedTask::getFixSizedTaskPointer(this);
        t->assertBeforeSpawn(); //奇怪，只要一打印t的各项状态，立刻就呈现正确的同步，观察则坍缩。不观察才出bug。要是不打印立刻就有大概率出错。
        auto len = end - beg;
        if (len < GrainSize){
            *sum = std::accumulate(beg, end, 0);
            assert(t->meta.pendingcnt.load()==0);
            assert(*sum>0);
            return;
        } 
        const int* mid = beg + len/2;
        int x=-123;
        int y=-123;
        assert(t->taskPointer()==this);
        //sleep(100);
        spawnPrivate<ParallelSum>(mid,end,&x);
		spawn<ParallelSum>(beg,mid,&y);
        localSync();
        t->assertAfterSync();
        assert(t->meta.synced==true);
        *sum=x+y;    
        if(x<0) println("x="+std::to_string(x));
        if(y<0) println("y="+std::to_string(y));
        if(*sum<0) println("sum="+std::to_string(*sum));
        //assert(x>0);
        //assert(y>0);
        //assert(*sum>0);
        //奇怪的bug：这里不加sleep或println会导致release版死锁，导致debug版core dumped。sleep(1)有时好有时坏。。
        //其原因是y,x,*sum都容易是负数！也就是说localSync莫名提前终止了！
        //无steal情景下不存在这个bug。那么就是steal后异线程sync不成功或不及时的问题。
        //甚至有时出现x,sum打印结果不对，最终结果仍对的情况，这就是其他线程改了对本线程暂时不可见的缘故！
        //那么得出结论，就是isSynchronised这个变量没设置memory order，导致A线程isSync修改前的sum变量修改对B线程isSync读取后的sum变量读取不可见。
        //println(">> sum="+std::to_string(x)+"+"+std::to_string(y));
    }
};


}