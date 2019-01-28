#pragma once

#include "cfsched.h"
#include <mutex>
#include "task.h"

namespace cfsched{

// TaskBuffer is a cache-friendly worker-local container of pre-constructed TaskrootTasks.
// It holds externally submitted tasks. It's much smaller than TaskDeque.
class TaskBuffer{
public:
    TaskBuffer(){
        for(auto& b : rootTasks) b.setLocation(FixSizedTask::atBufferFreeArea);
    }
    // Return the least recently emplaced task
    Task* steal()noexcept{
        std::lock_guard<std::mutex> lk(mtx);
        if(endPosition>beginPosition){
            beginPosition++;
            RootTask& rootTask=at(beginPosition-1);
            rootTask.setLocation(FixSizedTask::atBufferStolenArea); // Ready-->Stolen
            Task* userTask = rootTask.taskPointer();
            return userTask;
        }else{
            return nullptr;
        }
    }
    int size() const noexcept{
        std::lock_guard<std::mutex> lk(mtx);
        return endPosition-beginPosition;
    }   
    void reclaim(Task* executed)noexcept{
        // set Free
    }

    template < class T, class... Args >  
    T* emplace(Args&&... args){
        std::lock_guard<std::mutex> lk(mtx);
        if(endPosition-beginPosition >= Options::BufferSize){
            return nullptr; // return nullptr if current size == capacity
        }
        RootTask& rootTask=at(endPosition);
        if(rootTask.location()!=FixSizedTask::atBufferFreeArea){ // buffer has run out of space
            return nullptr;  
        }
        T* task = rootTask.emplaceTask<T>(std::forward<Args>(args)...); 
        rootTask.setLocation(FixSizedTask::atBufferReadyArea);  // Free-->Ready 
        endPosition++;
        return task;
    }
    void setWorkerid(uint8_t workerid) noexcept{ 
        for(auto& b : rootTasks) b.setWorkerid(workerid);
    }    
protected:
    // Random access should only be used internally under lock protection
    RootTask& at(uint64_t index) noexcept{
        return rootTasks[index%Options::BufferSize];
    }
private:
    RootTask                    rootTasks[Options::BufferSize];  // pre-constructed empty root tasks
    mutable std::mutex          mtx;
    uint64_t                    beginPosition=0; 
    uint64_t                    endPosition=0; // always >= beginPosition
};

}