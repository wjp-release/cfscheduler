#pragma once

#include <string>
#include "pool.h"

namespace cfsched{
    
/*==================================================
 Task functions both as a runnable in the pool that
 encapsulates user-defined task routine and a base 
 class that provides spwan/sync operations for user-defined subclasses.
 How to create tasks:
    1. define your task as Task's subclass
    2. override compute() with your task routine
    3. call Pool::instance().emplaceRoot to run the
    root task asynchronously
    4. call externalSync() if you want to block the current thread and sync with the root task
 In compute() function, you can use:
    1. localSync(): wait for all pending child tasks 
    2. spawnPrivate(): create a child task in execList,
    where tasks are protected from stealing 
    3. spawn(): create a child task in readyList
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
    T*                  spawnPrivate(Args&&... args)
    {
        return Pool::instance().emplaceExec<T>(FixSizedTask::getFixSizedTaskPointer(this), std::forward<Args>(args)...);
    }
    template < class T, class... Args >  
    T*                  spawn(Args&&... args){
        return Pool::instance().emplaceReady<T>(FixSizedTask::getFixSizedTaskPointer(this),std::forward<Args>(args)...);
    }
};



} 