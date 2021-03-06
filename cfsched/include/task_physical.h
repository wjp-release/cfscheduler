#pragma once
#include <cstdint>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <string>
#include <cassert>
#include "options.h"

namespace cfsched{

class Task;

class FixSizedTask{ 
public:
    struct FixSizedTaskMeta{ // very compact meta data. 
        FixSizedTaskMeta() : next(nullptr), refcnt(0), pendingcnt(0), synced(false) {}
        std::atomic<FixSizedTask*>  next; // intrusive linked list node 
        std::atomic<uint32_t>       refcnt; // used ONLY to avoid ABA problem in CAS lock-free stacks
        std::atomic<uint32_t>       pendingcnt; // how many unfinished subtasks it still waits for
        FixSizedTask*               parent = nullptr; // parent task
        volatile uint32_t           state = 0; 
        std::atomic<bool>           synced; // need release-acquire memory order such that updates are visible for sync-spinning threads
        static const uint32_t       locationMask=0x0000000f;
        static const uint32_t       workerIDMask=0x000000f0;
        static const uint32_t       isDoneMask=0x00000200;
    };
    enum TaskLocation : uint8_t{
        atFreeList          = 0,
        atReadyList         = 1,
        atExecList          = 2,
        atStolenList        = 3,
        atLocalStack        = 4,
        atBufferFreeArea    = 6,
        atBufferReadyArea   = 7,
        atBufferStolenArea  = 8,
    };
    uint8_t     location() const noexcept{
        return meta.state&meta.locationMask;
    }
    uint8_t     workerid() const noexcept{ 
        return (meta.state&meta.workerIDMask)>>4;
    }
    void        assertBeforeSpawn();
    void        assertAfterSync();
    bool        isSynchronised() const noexcept{ 
        return meta.synced.load(std::memory_order_acquire); // seq_cst or acquire
    }
    bool        isDone() const noexcept{ 
        return (meta.state&meta.isDoneMask)==meta.isDoneMask;
    }
    bool        isRoot() const noexcept{
        auto location_=location();
        return location_==6 || location_==7 || location_==8;
    }
    void        setIsDone(bool done) noexcept{
        if(done){
            meta.state|=meta.isDoneMask;
        }else{
            meta.state&=(~meta.isDoneMask);
        }
    }
    void        setIsSynchronised() noexcept{
        meta.synced.store(true, std::memory_order_release); // seq_cst better at debugging than release
    }
    void        setLocation(TaskLocation location) noexcept{
        meta.state=(location&meta.locationMask)|(meta.state&~meta.locationMask);
    }
    void        setWorkerid(uint8_t id) noexcept{ // should be set on construction of taskpools 
        meta.state=((id<<4)&meta.workerIDMask)|(meta.state&~meta.workerIDMask);
    }
    template < class T, class... Args >
    T*          emplaceTask(Args&&... args){ // emplace a user-defined task within taskAddress
        return new (taskAddress) T(std::forward<Args>(args)...);
    }
    template < class T > 
    void        destroyTask(){ // call task's destructor manually 
        reinterpret_cast<T*>(taskAddress)->~T();
    }
    template < class T = Task >
    T*          taskPointer() noexcept{
        return reinterpret_cast<T*>(taskAddress);
    }
    template < class T = Task >
    T&          taskReference() noexcept{
        return *reinterpret_cast<T*>(taskAddress);
    }
    void        print(const std::string& prefix) noexcept;
    void        printState() noexcept;
    void        reset() noexcept;
    void        setParentAndIncRefcnt(FixSizedTask*p);
    void        decreasePendingCount();
    void        tryDecreaseParentPendingCount();

    // Static functions that convert user task to a library task.
    static FixSizedTaskMeta&getMetaReference(Task* task) noexcept{
        return reinterpret_cast<FixSizedTaskMeta*>(task)[-1];
    }
    static FixSizedTaskMeta*getMetaPointer(Task* task) noexcept{
        return &(reinterpret_cast<FixSizedTaskMeta*>(task)[-1]);
    }
    static FixSizedTask&    getFixSizedTaskReference(Task* task) noexcept{
        return *reinterpret_cast<FixSizedTask*>(reinterpret_cast<FixSizedTaskMeta*>(task)-1);
    }
    static FixSizedTask*    getFixSizedTaskPointer(Task* task) noexcept{
        return reinterpret_cast<FixSizedTask*>(reinterpret_cast<FixSizedTaskMeta*>(task)-1);
    }
    // Data members whose order should not be modified.
    FixSizedTaskMeta meta;
    char taskAddress[Options::FixedSizeTaskSize-sizeof(FixSizedTaskMeta)];
};

// Root task needs mtx&cv to implement blocking wait. 
class RootTask : public FixSizedTask{ 
public:
    static RootTask&    getRootTaskReference(Task* task) noexcept{
        return *reinterpret_cast<RootTask*>(reinterpret_cast<FixSizedTaskMeta*>(task)-1);
    }
    static RootTask*    getRootTaskPointer(Task* task) noexcept{
        return reinterpret_cast<RootTask*>(reinterpret_cast<FixSizedTaskMeta*>(task)-1);
    }
    void                notifyAll(){ // wake up blocking threads
        cv.notify_all();
    }
    std::mutex mtx;
    std::condition_variable cv;
};

}