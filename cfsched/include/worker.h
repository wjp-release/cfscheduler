#pragma once

#include <string>
#include <thread>
#include "buffer.h"
#include "arena.h"

namespace cfsched{

/*==================================================
 Worker is an abstraction of a physical worker thread
 that includes its thread-local data, thread routine,
 and methods for task searching, monitoring and cpu 
 yielding.
 The task searching order is:
    1. take froom ExecList
    2. take from local ReadyList
    3. steal from other workers' ReadyList
    4. steal from local Buffer
    5. steal from other workers' Buffer
 Worker thread in a work-stealing scheme never blocks
 if it has work to do. But if all jobs are done, it 
 needs a cpu yielding strategy to avoid greedy busy
 spin. 
===================================================*/

class Task;

class Worker{
private:
    Task*           takeFromLocalReadyList();
    Task*           takeFromLocalExecList();
    Task*           stealFromLocalBuffer();
    Task*           stealFromOtherBuffers();
    Task*           stealFromBufferOf(Worker& worker);
    Task*           stealFromOtherDeques();
    Task*           stealFromDequeOf(Worker& worker);
    bool            executeTask(Task* task);
public:
    void            reclaim(Task* executed) noexcept;
    bool            findAndRunATask();
    void            findAndRunATaskOrYield();
    void            setWorkerid(uint8_t id) noexcept{ 
        this->id=id;
        buffer.setWorkerid(id);
        arena.setWorkerid(id);
    }
    void            tryWake(){
        // wake if it is sleeping
    }
    bool            holdsCurrentThread() const noexcept{
        return std::this_thread::get_id()==workerThread.get_id();
    }
    void            tryJoin(){
        if(workerThread.joinable()) workerThread.join();
    }
    std::string     stats();
    Buffer          buffer;
    Arena           arena;
    uint8_t         id;
    std::thread     workerThread;
};

}