#include "arena.h"
#include <sstream>
namespace cfsched{

std::string Arena::stats(){
    std::stringstream ss;

    return ss.str();
}

Arena::Arena(){
    for(auto& t:tasks) freeList.push(&t);
}

Task* Arena::steal(){
    FixSizedTask* t=readyList.pop();
    t->setLocation(FixSizedTask::atStolenList);
    return t->taskPointer();
}

Task* Arena::takeFromReady(){
    FixSizedTask* t=readyList.pop();
    t->setLocation(FixSizedTask::atLocalStack);
    return t->taskPointer();
}

Task* Arena::takeFromExec(){
    FixSizedTask* t=execList.pop();
    t->setLocation(FixSizedTask::atLocalStack);
    return t->taskPointer();
}

void Arena::reclaim(Task* executed) noexcept{
    FixSizedTask* t=FixSizedTask::getFixSizedTaskPointer(executed);
    auto location=t->location();
    switch(location){
    case FixSizedTask::atLocalStack:
        // Now we are going to put it back to freeList.
        break;
    case FixSizedTask::atStolenList:
        // We (stealer thread) are not responsible for 
        // reclaiming it. Its owning worker will run
        // gc routine in its own thread. So, we are
        // good now.
        break;
    case FixSizedTask::atExecList:
        assert(false&&"You should have been removed from execList before execution and reclamation!");
        break;
    case FixSizedTask::atReadyList:
        assert(false&&"You should have been removed from readyList before execution and reclamation!");
        break;
    case FixSizedTask::atFreeList:
        assert(false&&"So you have been freed? Who did it?");
        break;
    default:
        assert(false && "This task should not be reclaimed by a Arena!");
        break;
    }
    t->setLocation(FixSizedTask::atFreeList);
    freeList.push(t);
}

}