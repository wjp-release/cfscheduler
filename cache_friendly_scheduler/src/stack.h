#pragma once

#include <cstdint>
#include <atomic>

namespace cfsched{

struct Task;

class Stack
{
public:
    Stack() : stackHead(nullptr) {}
    void push(Task*);
    Task* pop(); // Return nullptr on failure.
    // Useful for traversing the list when there's no contention (e.g. to destroy remaining nodes)
    Task* head_unsafe() const { return stackHead.load(std::memory_order_relaxed); }
private:
    void add(Task*);
    static const uint32_t REFS_MASK = 0x7FFFFFFF;
    static const uint32_t SHOULD_BE_ON_FREELIST = 0x80000000;
    std::atomic<Task*> stackHead;
};




}