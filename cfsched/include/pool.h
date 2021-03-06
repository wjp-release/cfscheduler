#pragma once
#include "options.h"
#include "worker.h"
#include "utils.h"
#include <string>

namespace cfsched{

/*==================================================
 Pool is a global singleton task pool built upon
 Workers. It's compile-time configurable through 
"options.h". 
 You should call Pool::instance().start() before 
 emplacing any task. 
===================================================*/

class Pool{
public:
    Pool(const Pool&) = delete;
    void operator=(const Pool&) = delete;
	static Pool&        instance(){
		static Pool pool; 
		return pool;
	}
    Worker&             getWorker(int index) noexcept;
    Worker&             randomlyPickOne() noexcept;
    void                terminate() noexcept{
        terminating=true;
    }
    void                start() noexcept;
    std::string         who() const noexcept; //debug
    uint8_t             currentThreadIndex()const noexcept;
    uint8_t             getWorkerNumber()const noexcept{
        return workerNumber;
    }
    template < class T, class... Args >  
    T*                  emplaceRoot(Args&&... args)
    {
        Worker& w=randomlyPickOne();
        return w.buffer.emplace<T>(std::forward<Args>(args)...);
    }

    template < class T, class... Args >  
    T*                  emplaceExec(FixSizedTask*parent, Args&&... args)
    {
        return workers[currentThreadIndex()].arena.emplaceToExec<T>(parent, std::forward<Args>(args)...);
    }


    template < class T, class... Args >  
    T*                  emplaceReady(FixSizedTask*parent, Args&&... args)
    {
        return workers[currentThreadIndex()].arena.emplaceToReady<T>(parent, std::forward<Args>(args)...);
    }

    void                wakeAllSleepingWorkers()noexcept;
protected:
	Pool() noexcept;
    ~Pool();
private:
    volatile bool       terminating=false;  
    Worker*             workers;
    uint8_t             workerNumber;
    volatile bool       started=false;
};



}