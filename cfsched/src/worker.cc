#include "worker.h"
#include <sstream>
#include "task_logical.h"
#include "task_physical.h"
#include "utils.h"
#include "pool.h"

namespace cfsched{

std::string Worker::stats(){
    std::stringstream ss;
    ss<<"worker "<<(int)id;
    ss<<", arena stats: "<<arena.stats()<<", buffer stats: "<<buffer.stats();
    return ss.str();
}

void Worker::findAndRunATaskOrYield()
{
    if(!findAndRunATask()){
        // cpu yieldiing strategy
        switch(Options::YieldingStrategy){
        case CPUYieldingStrategy::LazyBusySpin:
            // do nothing
            break;
        case CPUYieldingStrategy::PauseForALittleWhile:
            sleep(Options::CPUYieldingPauseTime);
            break;
        case CPUYieldingStrategy::SleepUntilNextUserCall:
            // to be done
            break;
        default:
            break;
        }
    }
}

bool Worker::executeTask(Task* task)
{
    if(task==nullptr) return false;
    task->execute();  
    return true;
}

bool Worker::findAndRunATask()
{
    Task* task=nullptr;
    task = takeFromLocalExecList();         
    if(task) return executeTask(task);
    task = takeFromLocalReadyList();        
    if(task) return executeTask(task);
    task = stealFromOtherDeques();                
    if(task) return executeTask(task);
    task = stealFromLocalBuffer();          
    if(task) return executeTask(task);
    task = stealFromOtherBuffers();              
    if(task) return executeTask(task);
    return false;    
}


Task* Worker::takeFromLocalReadyList(){
    return arena.takeFromReady();
}

Task* Worker::takeFromLocalExecList(){
    return arena.takeFromExec();
}

Task* Worker::stealFromLocalBuffer(){
    return buffer.steal();
}

Task* Worker::stealFromOtherBuffers()
{
    Task* task=nullptr;
    auto nr_worker=Pool::instance().getWorkerNumber();
    for(uint8_t i=0;i<nr_worker;i++){
        if(i==id) continue;
        Worker& worker = Pool::instance().getWorker(i);
        task=stealFromBufferOf(worker);
        if(task!=nullptr){
            return task;
        } 
    }
    return task;
}

Task* Worker::stealFromBufferOf(Worker& worker)
{
    return worker.buffer.steal();
}

Task* Worker::stealFromOtherDeques()
{
    Task* task=nullptr;
    auto nr_worker=Pool::instance().getWorkerNumber();
    for(uint8_t i=0;i<nr_worker;i++){
        if(i==id) continue;
        Worker& worker = Pool::instance().getWorker(i);
        task=stealFromDequeOf(worker);
        if(task!=nullptr){ 
            return task;
        }
    }
    return task;
}

Task* Worker::stealFromDequeOf(Worker& worker)
{
    return worker.arena.steal();
}

void Worker::reclaim(Task* executed) noexcept
{
    FixSizedTask* t=FixSizedTask::getFixSizedTaskPointer(executed);
    if(t->isRoot()){
        buffer.reclaim(executed);
    }else{
        arena.reclaim(executed);
    }
}

}
