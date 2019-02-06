#pragma once

#include <cstdint>
#include <atomic>

namespace cfsched{

class FixSizedTask;

/*==================================================
 Stack is stack structure based on Moody Camel's
 lock-free ABA-free intrusive singly linked list.
===================================================*/

class Stack
{
public:
    Stack() : stackHead(nullptr) {}
    void            push(FixSizedTask*);
    FixSizedTask*   pop(); // Return nullptr on failure.
  private:
    void add(FixSizedTask*);
    static const uint32_t REFS_MASK = 0x7FFFFFFF;
    static const uint32_t SHOULD_BE_ON_FREELIST = 0x80000000;
    std::atomic<FixSizedTask*> stackHead;
};




}