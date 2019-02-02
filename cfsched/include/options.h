#pragma once
#include <cstdint>

#define EnableInternalMonitor

namespace cfsched{

struct Options{
    static const uint32_t FixedSizeTaskSize = 64; 
    static const uint32_t BufferSize = 64;
    static const uint32_t ArenaSize = 1024*1024;
};

}