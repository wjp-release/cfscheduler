#include "task_logical.h"
#include "task_physical.h"
#include <string>

namespace cfsched{

void Task::execute(){
    compute();
    auto t=FixSizedTask::getFixSizedTaskPointer(this);
    t->setIsDone(true);
    t->tryDecreaseParentPendingCount();
    if(t->isRoot()){
        RootTask::getRootTaskPointer(this)->notifyAll();
    }
    if( t->location()==FixSizedTask::atLocalStack && (!t->isRoot()) ){
        Pool::instance().getWorker(Pool::instance().currentThreadIndex()).reclaim(this);
    }
}

std::string Task::stats(){
    return "Task<"+std::to_string(reinterpret_cast<uint64_t>(this))+">";
}

void Task::externalSync()
{
    auto t=RootTask::getRootTaskPointer(this);
    assert(t->isRoot());
    std::unique_lock<std::mutex> lk(t->mtx);
    while(!t->isDone()){
        t->cv.wait(lk);
    }
    Pool::instance().getWorker(t->workerid()).reclaim(this);
}

void Task::localSync()
{
    auto t=FixSizedTask::getFixSizedTaskPointer(this);
    while(!t->isSynchronised()){
        Pool::instance().getWorker(Pool::instance().currentThreadIndex()).findAndRunATask();
    }
}

}