#pragma once

#include "task_physical.h"
#include "stack.h"
#include "utils.h"
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
    atLocalStack: no longer tracked by any list;
    ownership transferred to local worker thread        
===================================================*/
class Arena{
public:
    Arena();
    std::string     stats();
    Task*           steal();
    void            gc(); // stolen list
    Task*           takeFromReady();
    Task*           takeFromExec();
    void            reclaim(Task* executed) noexcept;
    void            setWorkerid(uint8_t id) noexcept{ 
        for(auto&t:tasks) t.setWorkerid(id); 
        freeList.setid(id);
        execList.setid(id);
    }
    template < class T, class... Args >  
    T*              emplaceToExec(FixSizedTask*parent, Args&&... args){
        FixSizedTask* addr=freeList.pop();
        if(addr==nullptr){
    #ifdef EnableDegbugging   
            println("freelist size="+std::to_string(freeList.size()));
            assert(false&&"gc untested!");
    #endif
            gc();
            addr=freeList.pop();
        }
        if(addr==nullptr){
    #ifdef EnableDegbugging        
            println("still no space!");
    #endif
            return nullptr;
        }
        T* task = new (addr->taskPointer()) T(std::forward<Args>(args)...);
        FixSizedTask* t=FixSizedTask::getFixSizedTaskPointer(task);
        t->setParentAndIncRefcnt(parent);
    #ifdef EnableDegbugging        
        assert(t->meta.pendingcnt.load()==0);
        assert(t->meta.synced.load()==false);
    #endif
        pushToExecList(t);
        return task;        
    }
    template < class T, class... Args >  
    T*              emplaceToReady(FixSizedTask*parent, Args&&... args){
        FixSizedTask* addr=freeList.pop();
        if(addr==nullptr){
    #ifdef EnableDegbugging        
            println("freelist size="+std::to_string(freeList.size()));
            assert(false&&"gc untested!");
    #endif
            gc();
            addr=freeList.pop();
        }
        if(addr==nullptr){
    #ifdef EnableDegbugging        
            println("still no space!");
            return nullptr;
    #endif
        }       
        T* task = new (addr->taskPointer()) T(std::forward<Args>(args)...);
        FixSizedTask* t=FixSizedTask::getFixSizedTaskPointer(task);
        t->setParentAndIncRefcnt(parent);
    #ifdef EnableDegbugging        
        assert(t->meta.pendingcnt.load()==0);
        assert(t->meta.synced.load()==false);
    #endif
        pushToReadyList(t);
        return task;        
    }  
private:
    void            pushToStolenList(FixSizedTask*t){
        t->setLocation(FixSizedTask::atStolenList);
        stolenList.push(t);
    }
    void            pushToFreeList(FixSizedTask*t){
        t->setLocation(FixSizedTask::atFreeList);
        freeList.push(t);
    }
    void            pushToReadyList(FixSizedTask*t){
        t->setLocation(FixSizedTask::atReadyList);
        readyList.push(t);
    }
    void            pushToExecList(FixSizedTask*t){
        t->setLocation(FixSizedTask::atExecList);
        execList.push(t);
    }
    FixSizedTask    tasks[Options::ArenaSize];
    PrivateStack    freeList; 
    ABAProneStack   readyList;  
    PrivateStack    execList; 
    ABAProneStack   stolenList; //ABAProneStack is faster, dangerous but not wrong
};

}