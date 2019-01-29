#include "task.h"
#include <string>

namespace cfsched{

void Task::execute(){
    compute();
    // parent dec refcnt
    auto systemTask=FixSizedTask::getFixSizedTaskPointer(this);
    systemTask->tryDecreaseParentPendingCount();
    // if task is root, signal it
    if(systemTask->isRoot()){
        RootTask* root=RootTask::getRootTaskPointer(this);
        root->notifyAll();        
    }
    // reclaim memory

    // reset some data members, except those 

}

std::string Task::stats(){
    return "Task<"+std::to_string(reinterpret_cast<uint64_t>(this))+">";
}


}