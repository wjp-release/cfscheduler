#pragma once

#ifdef EnableProfiling

#include <atomic>
#include "utils.h"

namespace cfsched{

class Profiler{
public:
    Profiler(const Profiler&) = delete;
    void operator=(const Profiler&) = delete;
	static Profiler& instance(){
		static Profiler p; 
		return p;
	}
    std::atomic<int> executedTaskCount;
    std::atomic<int> spawnedTaskCount;
    void print(){
        println("spawned task count="+std::to_string(executedTaskCount.load()));
        println("executed task count="+std::to_string(executedTaskCount.load()));
    }
private:
    Profiler() : executedTaskCount(0),spawnedTaskCount(0) {}
};

#endif

}