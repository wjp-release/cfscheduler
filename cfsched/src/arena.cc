#include "arena.h"
#include <sstream>
#include "utils.h"
namespace cfsched{

std::string Arena::stats(){
    std::stringstream ss;
    ss<<"execList: "<<execList.stats()<<", stolenList: "<<stolenList.stats()<<", readyList: "<<readyList.stats()<<"\n";
    return ss.str();
}

Arena::Arena(){
    for(auto& t:tasks){
        pushToFreeList(&t);
    } 
}

Task* Arena::steal(){
    FixSizedTask* t=readyList.pop();
    if(t==nullptr) return nullptr;
    pushToStolenList(t);
    return t->taskPointer();
}

Task* Arena::takeFromReady(){
    FixSizedTask* t=readyList.pop();
    if(t==nullptr) return nullptr;
    t->setLocation(FixSizedTask::atLocalStack);
    return t->taskPointer();
}

Task* Arena::takeFromExec(){
    FixSizedTask* t=execList.pop();
    if(t==nullptr) return nullptr;
    t->setLocation(FixSizedTask::atLocalStack);
    return t->taskPointer();
}

void Arena::gc(){
    println("garbage collection！");
    PrivateStack tmp;
    FixSizedTask* task;
    int patience=3;
    while(patience>0){
        task=stolenList.pop();
        if(task==nullptr){
            patience--;
            continue;
        }
        if(task->isDone()){
            task->setLocation(FixSizedTask::atFreeList);
            freeList.push(task);
        }else{
            tmp.push(task);
        }
    }
    // Put unfinished tasks back to stolenList
    FixSizedTask* unfinished=tmp.pop();
    while(unfinished!=nullptr){
        stolenList.push(unfinished);
        unfinished=tmp.pop();
    }
}

void Arena::reclaim(Task* executed) noexcept{
    auto t=FixSizedTask::getFixSizedTaskPointer(executed);
    if(t->location()==FixSizedTask::atStolenList) return;
    t->reset();
    pushToFreeList(t);
}

}