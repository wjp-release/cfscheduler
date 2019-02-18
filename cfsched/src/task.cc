#include "task_logical.h"
#include "task_physical.h"
#include <string>
#include <cstdio>
#include "utils.h"
#include "pool.h"
#include "profiler.h"

namespace cfsched{


void Task::execute(){
#ifdef EnableProfiling
    Profiler::instance().executedTaskCount++;
#endif
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
    if(t->meta.pendingcnt.load()!=2 && t->meta.pendingcnt.load()!=1){
        t->print("localSync() entry: ");
    }
    while(!t->meta.synced.load()){
        Pool::instance().getWorker(Pool::instance().currentThreadIndex()).findAndRunATask();
    }
    assert(t->meta.synced.load());
    if(t->meta.pendingcnt.load()!=0){
        t->print("localSync() end: ");
    }
}

/*=====================================================*/

void FixSizedTask::decreasePendingCount(){
    uint32_t pendingcnt= meta.pendingcnt.fetch_sub(1);
    if(pendingcnt==1){ // last child done
        setIsSynchronised();
        assert(meta.pendingcnt.load()==0 && "now pendingcnt should be zero");
        assert(meta.synced.load()==true);
    } 
}

void FixSizedTask::setParentAndIncRefcnt(FixSizedTask*p){
    meta.parent=p;
    p->meta.pendingcnt.fetch_add(1);
}

void FixSizedTask::assertBeforeSpawn(){
    if(meta.pendingcnt.load()!=0 || meta.synced.load()==true) print("before spawn: ");
}

void FixSizedTask::assertAfterSync(){
    if(meta.pendingcnt.load()!=0|| meta.synced.load()==false) print("after sync: ");
}

void FixSizedTask::print(const std::string& prefix) noexcept{
    println(prefix+"location="+std::to_string(location())+", isDone="+std::to_string(isDone())+", workerid="+std::to_string(workerid())+", pendincnt="+std::to_string( meta.pendingcnt.load())+", refcnt="+std::to_string(meta.refcnt.load())
    +", synced="+std::to_string(meta.synced.load())
    +", executorid="+Pool::instance().who());
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
    meta.synced.store(false);
}

void FixSizedTask::tryDecreaseParentPendingCount(){
    if(meta.parent){
        meta.parent->decreasePendingCount();
    }
}


}