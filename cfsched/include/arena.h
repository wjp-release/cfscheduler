#pragma once

#include <string>
#include "task_physical.h"
#include "stack.h"

namespace cfsched{


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
    std::string     stats();
    void            setWorkerid(uint8_t id) noexcept{ 
        for(auto&t:tasks) t.setWorkerid(id);  
    }

    template < class T, class... Args >  
    T*          emplace(Args&&... args){
        //todo
        return nullptr;
    }
private:
    FixSizedTask tasks[Options::BufferSize];
};

}