#include "buffer.h"

namespace cfsched{

// Return the least recently emplaced task
Task* Buffer::steal()noexcept{
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

int Buffer::size() const noexcept{
    std::lock_guard<std::mutex> lk(mtx);
    return endPosition-beginPosition;
}   

void Buffer::reclaim(Task* executed)noexcept{
    FixSizedTask& fixSizedTask=FixSizedTask::getFixSizedTaskReference(executed);
    fixSizedTask.setLocation(FixSizedTask::atBufferFreeArea);
}



}