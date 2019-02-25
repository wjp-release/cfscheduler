#pragma once

#include <cstdint>
#include <atomic>
#include <string>
#include <mutex>
#include "options.h"

namespace cfsched{

class FixSizedTask;

/*==================================================
 Stack structures based on intrusive singly linked 
 lists. 
===================================================*/

class Stack
{
public:
    Stack() : stackHead(nullptr) {}
    void            push(FixSizedTask*);
    FixSizedTask*   pop(); //return nullptr on failure.
    std::string     stats(); //debugging stats
  private:
    void add(FixSizedTask*);
    static const uint32_t REFS_MASK = 0x7FFFFFFF;
    static const uint32_t SHOULD_BE_ON_FREELIST = 0x80000000;
    std::atomic<FixSizedTask*> stackHead;
};

class BlockingStack
{
public:
    BlockingStack() : stackHead(nullptr) {}
    void            push(FixSizedTask*);
    FixSizedTask*   pop(); //return nullptr on failure.
    std::string     stats(); //debugging stats
  private:
    FixSizedTask*   stackHead;
    std::mutex      mtx;
};

class PrivateStack
{
public:
    PrivateStack() : stackHead(nullptr) {}
    void            push(FixSizedTask*);
    FixSizedTask*   pop(); //return nullptr on failure.
    int             size(); 
    void            setid(uint8_t id){
    #ifdef EnableDebugging
        workerid=id;
    #endif
    }
    std::string     stats();//debugging stats
private:
#ifdef EnableDebugging
    int             count=0;
    uint8_t         workerid=0;
#endif
    FixSizedTask*   stackHead;
};


class ABAProneStack
{
public:
    ABAProneStack() : stackHead(nullptr) {}
    void            push(FixSizedTask*);
    FixSizedTask*   pop(); //return nullptr on failure.
    std::string     stats(); //debugging stats
  private:
    std::atomic<FixSizedTask*> stackHead;
    std::mutex      mtx;
};

}