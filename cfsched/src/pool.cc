#include "pool.h"
#include "utils.h"
#include "worker.h"

namespace cfsched{

Pool::Pool() noexcept :terminating(false), started(false), workerNumber(nr_cpu()){
    workers=new Worker[workerNumber];
    for(uint8_t i=0;i<workerNumber;i++){
        workers[i].setWorkerid(i);
        workers[i].workerThread=std::thread{
            [this,i]{
                while(!started){ 
                    if(terminating) return;
                }
            #ifdef EnableDebugging
                println("started!");
            #endif
                while(!terminating){
            #ifdef EnableExcept
                    try{
                        workers[i].findAndRunATaskOrYield();
                    }catch(std::exception& e){ 
                        println("Worker"+std::to_string(i)+e.what());
                    }catch(...){
                        println("Worker"+std::to_string(i)+" unknown exception");
                    }
            #else
                workers[i].findAndRunATaskOrYield();
            #endif    
                }
            #ifdef EnableDebugging
                println("terminated!");
            #endif
            }
        };
    }
}

Pool::~Pool()
{
    terminating=true;
    wakeAllSleepingWorkers();
    for(int i=0; i<workerNumber; i++){
        workers[i].tryJoin();
    }
    delete [] workers;
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

std::string Pool::who() const noexcept
{
    return "worker"+std::to_string(cfsched::Pool::instance().currentThreadIndex())+" ";
}

void Pool::wakeAllSleepingWorkers()noexcept
{
    for(int i=0; i<workerNumber; i++){
        workers[i].tryWake();
    }
}

void Pool::start() noexcept{
    if(started) return;
    started=true;
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