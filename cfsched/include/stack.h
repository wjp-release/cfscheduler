#pragma once

#include <cstdint>
#include <atomic>
#include <string>

namespace cfsched{

class FixSizedTask;

/*==================================================
 Stack is stack structure based on Moody Camel's
 lock-free ABA-free intrusive singly linked list.
 PrivateStack is a thread-unsafe version of Stack.
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

class PrivateStack
{
public:
    PrivateStack() : stackHead(nullptr) {}
    void            push(FixSizedTask*);
    FixSizedTask*   pop(); //return nullptr on failure.
    std::string     stats();//debugging stats
private:
    FixSizedTask*   stackHead;
};



}