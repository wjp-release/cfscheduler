#pragma once
#include <cstdint>

//#define EnableInternalMonitor
//#define EnableProfiling
//#define EnableDegbugging
//#define DisableReclamation
//#define EnableExcept

namespace cfsched{

enum class CPUYieldingStrategy{
     LazyBusySpin,
     PauseForALittleWhile,
     SleepUntilNextUserCall
};

struct Options{
    static const uint32_t FixedSizeTaskSize = 128; 
    static const uint32_t BufferSize = 64;
    static const uint32_t ArenaSize = 1024*1024;
    static const CPUYieldingStrategy YieldingStrategy= CPUYieldingStrategy::LazyBusySpin; 
    static const uint32_t CPUYieldingPauseTime = 100;
    static const bool EnableSanityCheck = false;
};

}