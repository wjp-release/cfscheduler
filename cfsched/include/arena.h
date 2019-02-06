#pragma once

#include "task_physical.h"
#include "stack.h"
#include <cassert>
#include <string>

namespace cfsched{

class Task;

/*==================================================
 Arena is Worker's private container of FixSizedTasks,
 which are child tasks spawned by parents run by the
 worker thread. 
 It functions as a memory pool, a free list, and 
 a lock-free work-stealing data structure at the same
 time. 
 FixSizedTasks in Arena fall into 6 categories: 
    atFreeList: not allocated          
    atReadyList: vulnerable to stealing         
    atExecList: protected from stealing         
    atStolenList: stolen by other workers     
    atLocalStack: run by local worker thread        
===================================================*/
class Arena{
public:
    Arena();
    std::string     stats();
    Task*           steal();
    Task*           takeFromReady();
    Task*           takeFromExec();
    void            reclaim(Task* executed) noexcept;
    void            setWorkerid(uint8_t id) noexcept{ 
        for(auto&t:tasks) t.setWorkerid(id);  
    }
    // free --> exec (protected from stealers)
    template < class T, class... Args >  
    T*              emplaceToExec(FixSizedTask*parent, Args&&... args){
        FixSizedTask* addr=freeList.pop();
        T* task = new (addr) T(std::forward<Args>(args)...);
        FixSizedTask* t=FixSizedTask::getFixSizedTaskPointer(task);
        t->setParentAndIncRefcnt(parent);
        execList.push(t);
        return task;        
    }

      // free --> exec (protected from stealers)
    template < class T, class... Args >  
    T*              emplaceToReady(FixSizedTask*parent, Args&&... args){
        FixSizedTask* addr=freeList.pop();
        T* task = new (addr) T(std::forward<Args>(args)...);
        FixSizedTask* t=FixSizedTask::getFixSizedTaskPointer(task);
        t->setParentAndIncRefcnt(parent);
        readyList.push(t);
        return task;        
    }  
private:
    FixSizedTask    tasks[Options::ArenaSize];
    Stack           freeList;
    Stack           readyList;
    Stack           execList;
    Stack           stolenList;
};

}