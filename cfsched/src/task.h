#pragma once
#include <cstdint>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "cfsched.h"
namespace cfsched{
struct FixSizedTask{ 
    struct FixSizedTaskMeta{ // very compact meta data. 
        FixSizedTaskMeta() : next(nullptr), refcnt(0) {}
        std::atomic<FixSizedTask*> next; // intrusive linked list node 
        std::atomic<uint32_t> refcnt; // used ONLY to avoid ABA problem in CAS lock-free stacks
        std::atomic<uint32_t> pendingcnt; // how many unfinished subtasks it still waits for
        FixSizedTask* parent = nullptr; // parent task
        uint32_t state = 0;
        static const uint32_t locationMask=0x0000000f;
        static const uint32_t workerIDMask=0x000000f0;
        static const uint32_t isSynchronisedMask=0x00000100;
        static const uint32_t isDoneMask=0x00000200;
    };
    enum TaskLocation : uint8_t{
        atFreeList = 0,
        atReadyList = 1,
        atExecList = 2,
        atStolenList = 3,
        atLocalStack = 4,
        atStealerStack = 5,
        atBufferFreeArea = 6,
        atBufferReadyArea = 7,
        atBufferStolenArea = 8,
    };
    uint8_t location() const noexcept{ // where the task is 
        return meta.state&meta.locationMask;
    }
    uint8_t workerid() const noexcept{ // which worker it belongs to
        return (meta.state&meta.workerIDMask)>>4;
    }
    bool isSynchronised() const noexcept{ // isSynchronised: all subtasks are done.
        return (meta.state&meta.isSynchronisedMask)==meta.isSynchronisedMask;
    }
    bool isDone() const noexcept{ // isDone: a mark for delayed garbage collection.
        return (meta.state&meta.isDoneMask)==meta.isDoneMask;
    }
    void setIsDone(bool done) noexcept{
        if(done){
            meta.state|=meta.isDoneMask;
        }else{
            meta.state&=(~meta.isDoneMask);
        }
    }
    void setIsSynchronised(bool done) noexcept{
        if(done){
            meta.state|=meta.isSynchronisedMask;
        }else{
            meta.state&=(~meta.isSynchronisedMask);
        }
    }
    void setLocation(TaskLocation location) noexcept{
        meta.state=(location&meta.locationMask)|(meta.state&~meta.locationMask);
    }
    void setWorkerid(uint8_t id) noexcept{ // should be set on construction of taskpools 
        meta.state=((id<<4)&meta.workerIDMask)|(meta.state&~meta.workerIDMask);
    }
    template < class T, class... Args >
    T* emplaceTask(Args&&... args){ // emplace a user-defined task within taskAddress
        return new (taskAddress) T(std::forward<Args>(args)...);
    }
    template < class T > 
    void destroyTask(){ // call task's destructor manually 
        reinterpret_cast<T*>(taskAddress)->~T();
    }
    template < class T = Task >
    T* taskPointer() noexcept{
        return reinterpret_cast<T*>(taskAddress);
    }
    template < class T = Task >
    T& taskReference() noexcept{
        return *reinterpret_cast<T*>(taskAddress);
    }
    void reset() noexcept{ // reset data members before recycling into freelist again
        //@todo
    }
    FixSizedTaskMeta meta;
    char taskAddress[Options::FixedSizeTaskSize-sizeof(FixSizedTaskMeta)]; // padded to 64/128/256 bytes
};
// Root task needs mtx&cv to implement blocking wait. 
struct RootTask : public FixSizedTask{ 
    std::mutex mtx;
    std::condition_variable cv;
};




} 