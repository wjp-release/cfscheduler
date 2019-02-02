#pragma once

#include <string>
#include "pool.h"

namespace cfsched{
    
/*==================================================
 Task functions both as a runnable in the pool that
 encapsulates user-defined task routine and a base 
 class that provides spwan/sync operations for user-defined subclasses.
 How to use it:
    1. define your task as Task's subclass
    2. override compute() as your task routine
    3. call Pool::instance().emplaceRoot to kick it
    off asynchronously in the pool
    4. call externalSync() if you want to block the current thread and sync with the task.
 In compute() function, you can use:
    1. localSync(): wait for all pending child tasks 
    2. spawnDetached(): create a detached task that
    is not counted as its pending child task.
    3. spawn(): create a pending child task, increase
    pending count.
===================================================*/

class Task{
public:
    void                execute();
    virtual std::string stats();
    void                externalSync();
    void                localSync();
protected:
    virtual void        compute()=0;
     
    template < class T, class... Args >  
    T*                  spawnDetached(Args&&... args){
        //todo
        return nullptr;
    }
};



} 