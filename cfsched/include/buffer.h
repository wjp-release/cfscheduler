#pragma once

#include <mutex>
#include "task_physical.h"

namespace cfsched{

/*==================================================
 Buffer is Worker's private container of RootTasks, 
 which are normally created by user threads.
 Workers will steal tasks from its own buffer and even
 other workers' buffer if its arena becomes empty.
 RootTasks in Buffer fall into 3 categories: 
    atBufferFreeArea
    atBufferReadyArea  
    atBufferStolenArea  
 Buffer::steal() returns the least recently emplaced 
 task.
===================================================*/

class Buffer{
public:
    Buffer(){
        for(auto& b : rootTasks) b.setLocation(FixSizedTask::atBufferFreeArea);
    }
    Task*       steal()noexcept;
    int         size() const noexcept;
    void        reclaim(Task* executed)noexcept;
    std::string stats();

    template < class T, class... Args >  
    T*          emplace(Args&&... args){
        std::lock_guard<std::mutex> lk(mtx);
        if(endPosition-beginPosition >= Options::BufferSize){
            return nullptr; 
        }
        RootTask& rootTask=at(endPosition);
        if(rootTask.location()!=FixSizedTask::atBufferFreeArea){ 
            return nullptr;  
        }
        T* task = rootTask.emplaceTask<T>(std::forward<Args>(args)...); 
        rootTask.setLocation(FixSizedTask::atBufferReadyArea); 
        endPosition++;
        return task;
    }
    void        setWorkerid(uint8_t id)noexcept{ 
        for(auto&t:rootTasks) t.setWorkerid(id);
    }    
protected:
    RootTask&   at(uint64_t index) noexcept{
        return rootTasks[index%Options::BufferSize];
    }
private:
    RootTask    rootTasks[Options::BufferSize];  
    mutable std::mutex mtx;
    uint64_t    beginPosition=0; 
    uint64_t    endPosition=0; 
};

}