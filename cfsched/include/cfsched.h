#pragma once

#include <cstdint>
#include <string>

namespace cfsched{

struct Options{
    // Compile time options 

    // FixedSizeTask is the data structure that manages states of a task in cfsched pool.
    // sizeof(Task)+sizeof(FixedSizeTask::FixedSizeTaskMeta)+padding bytes == FixedSizeTaskSize
    static const uint32_t FixedSizeTaskSize = 64; 

    // If none of your tasks have destructors, this flag should be set false.
    static const bool TaskHasDestructor = false; 

    // Note that Buffer is a data structure that holds externally created root tasks.
    // Each worker has one buffer. 
    static const uint32_t BufferSize = 1024;

    // Runtime options


};

// User-defined Task Routine 
class Task{
public:
    // compute and clean up
    void execute();
    // overridable debugging info
    virtual std::string stats();
    // called by a user thread
    void externalSync();
    // called by a worker thread
    void localSync();
protected:
    // user-defined task routine
    virtual void compute()=0;
};




}