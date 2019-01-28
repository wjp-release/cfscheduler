#pragma once

#include <cstdint>

namespace cfsched{

struct Options{
    // FixedSizeTask is the data structure that manages states of a task in cfsched pool.
    // sizeof(Task)+sizeof(FixedSizeTask::FixedSizeTaskMeta)+padding bytes == FixedSizeTaskSize
    static const uint32_t FixedSizeTaskSize = 64; 

    // If none of your tasks have destructors, this flag should be set false.
    static const bool TaskHasDestructor = false; 

    // Note that Buffer is a data structure that holds externally created root tasks.
    // Each worker has one buffer. 
    static const uint32_t BufferSize = 1024;

};

// User-defined Task Routine 
class Task{


};


}