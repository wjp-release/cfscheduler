#pragma once

#include <cstdint>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace cfsched{
 
struct Task{
    Task() : next(nullptr), refcnt(0), state(0) {}
    uint8_t location() const noexcept{
        return state&locationMask;
    }
    uint8_t workerid() const noexcept{
        return (state&workerIDMask)>>4;
    }
    bool isSynchronised() const noexcept{ // isSynchronised: all subtasks are done.
        return (state&isSynchronisedMask)==isSynchronisedMask;
    }
    bool isDone() const noexcept{ // isDone: a mark for delayed garbage collection.
        return (state&isDoneMask)==isDoneMask;
    }
    void setIsDone(bool done) noexcept{
        if(done){
            state|=isDoneMask;
        }else{
            state&=(~isDoneMask);
        }
    }
    void setIsSynchronised(bool done) noexcept{
        if(done){
            state|=isSynchronisedMask;
        }else{
            state&=(~isSynchronisedMask);
        }
    }
    void setLocation(uint8_t location) noexcept{
        state=(location&locationMask)|(state&~locationMask);
    }
    void setWorkerid(uint8_t id) noexcept{
        state=((id<<4)&workerIDMask)|(state&~workerIDMask);
    }
    std::atomic<Task*> next;
    std::atomic<uint32_t> refcnt;
    uint32_t state;
    static const uint32_t locationMask=0x0000000f;
    static const uint32_t workerIDMask=0x000000f0;
    static const uint32_t isSynchronisedMask=0x00000100;
    static const uint32_t isDoneMask=0x00000200;
};

// RootTask has additional data members, such as mtx and convar, 
// to support blocking external wait. 
// Internal wait is nonblocking, some space from 
struct RootTask : public Task{
    std::mutex                  mtx;
    std::condition_variable     cv;
};


}