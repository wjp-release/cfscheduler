#include "pool.h"
#include "utils.h"
#include "worker.h"

namespace cfsched{

Pool::Pool() noexcept :terminating(false), workerNumber(nr_cpu()){
    println("pool constructing...");
    workers=new Worker[workerNumber];
    for(uint8_t i=0;i<workerNumber;i++){
        workers[i].setWorkerid(i);
    }
    println("pool constructed！");
}

Pool::~Pool()
{
    println("pool terminating...");
    terminating=true;
    wakeAllSleepingWorkers();
    for(int i=0; i<workerNumber; i++){
        workers[i].tryJoin();
    }
    delete [] workers;
    println("pool terminated!");
}


Worker& Pool::getWorker(int index) noexcept{
    return workers[index];    
}

Worker& Pool::randomlyPickOne() noexcept{
    return workers[randinteger(0, workerNumber-1)];
}

uint8_t Pool::currentThreadIndex()const noexcept
{
    for(int i=0; i<workerNumber; i++){
        if(workers[i].holdsCurrentThread()) return i;
    }
    return -1;
}

void Pool::wakeAllSleepingWorkers()noexcept
{
    for(int i=0; i<workerNumber; i++){
        workers[i].tryWake();
    }
}

void Pool::start(){
    for(int i=0;i<workerNumber;i++){
        println("starting worker"+std::to_string(i)+"...");
        workers[i].id=i;
        workers[i].workerThread=std::thread{
            [this,i]{
                while(!terminating){
                    try{
                        workers[i].findAndRunATaskOrYield();
                    }catch(std::exception& e){ 
                        println("Worker"+std::to_string(i)+e.what());
                    }catch(...){
                        println("Worker"+std::to_string(i)+" unknown exception");
                    }
                }
            }
        };
        println("worker"+std::to_string(i)+" started!");
    }
    #ifdef EnableInternalMonitor
    std::thread([this]{
        while(true){
            sleep(1000);
            println("\nMonitoring "+std::to_string(workerNumber)+" workers:");
            for(int i=0;i<workerNumber;i++){
                println(workers[i].stats());
            }
        }
    }).detach();
    #endif
}


}