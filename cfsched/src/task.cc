#include "task_logical.h"
#include "task_physical.h"
#include <string>
#include <cstdio>
#include "utils.h"
#include "pool.h"

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
        bool found=Pool::instance().getWorker(Pool::instance().currentThreadIndex()).findAndRunATask();
    }
}

/*=====================================================*/

void FixSizedTask::decreasePendingCount(){
    uint32_t pendingcnt= meta.pendingcnt.fetch_sub(1);
    if(pendingcnt==1){ // last child done
        setIsSynchronised(true);
    } 
}

void FixSizedTask::setParentAndIncRefcnt(FixSizedTask*p){
    meta.parent=p;
    p->meta.pendingcnt.fetch_add(1);
}

void FixSizedTask::print() noexcept{
    printf("%s's pendingcnt=%d, recnt=%d, next=%lld, parent=%lld\n", (Pool::instance().who()+taskPointer()->stats()).c_str(), meta.pendingcnt.load(), meta.refcnt.load(),(long long)meta.next.load(),(long long)meta.parent);
}

void FixSizedTask::printState() noexcept{
    printf("state=0x%x\n", meta.state);
}

void FixSizedTask::reset() noexcept{ 
    meta.state=0;
    meta.parent=nullptr;
    meta.pendingcnt.store(0);
    meta.refcnt.store(0);
    meta.next.store(0);
}

void FixSizedTask::tryDecreaseParentPendingCount(){
    if(meta.parent){
        meta.parent->decreasePendingCount();
    }
}


}