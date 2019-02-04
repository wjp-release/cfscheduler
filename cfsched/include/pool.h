#pragma once
#include "options.h"
#include "worker.h"
#include "utils.h"

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
    void                start();
    uint8_t             currentThreadIndex()const noexcept;

    template < class T, class... Args >  
    T*                  emplaceRoot(Args&&... args)
    {
        Worker& w=randomlyPickOne();
        return w.buffer.emplace<T>(std::forward<Args>(args)...);
    }

    template < class T, class... Args >  
    T*                  emplaceExec(uint8_t index, Args&&... args)
    {
        return workers[index].arena.emplaceToExec<T>(std::forward<Args>(args)...);
    }


    template < class T, class... Args >  
    T*                  emplaceReady(uint8_t index, Args&&... args)
    {
        return workers[index].arena.emplaceToReady<T>(std::forward<Args>(args)...);
    }

    void                wakeAllSleepingWorkers()noexcept;
protected:
	Pool() noexcept;
    ~Pool();
private:
    volatile bool       terminating;  
    Worker*             workers;
    uint8_t             workerNumber;
};



}