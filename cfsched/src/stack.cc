#include "stack.h"
#include "task_physical.h"
#include "task_logical.h"
#include "pool.h"

namespace cfsched{

void Stack::add(FixSizedTask* node)
{
    // Since the refcount is zero, and nobody can increase it once it's zero (except us, and we
    // run only one copy of this method per node at a time, i.e. the single thread case), then we
    // know we can safely change the next pointer of the node; however, once the refcount is back
    // above zero, then other threads could increase it (happens under heavy contention, when the
    // refcount goes to zero in between a load and a refcount increment of a node in try_get, then
    // back up to something non-zero, then the refcount increment is done by the other thread) --
    // so, if the CAS to add the node to the actual list fails, decrease the refcount and leave
    // the add operation to the next thread who puts the refcount back at zero (which could be us,
    // hence the loop).
    auto head = stackHead.load(std::memory_order_relaxed);
    while (true) {
        node->meta.next.store(head, std::memory_order_relaxed);
        node->meta.refcnt.store(1, std::memory_order_release);
        if (!stackHead.compare_exchange_strong(head, node, std::memory_order_release, std::memory_order_relaxed)) {
            if (node->meta.refcnt.fetch_add(SHOULD_BE_ON_FREELIST - 1,
                    std::memory_order_release) == 1) {
                continue;
            }
        }
        return;
    }
}

void Stack::push(FixSizedTask* node)
{
    // We know that the should-be-on-freelist bit is 0 at this point, so it's safe to
    // set it using a fetch_add
    if (node->meta.refcnt.fetch_add(SHOULD_BE_ON_FREELIST, std::memory_order_release) == 0) {
        // Oh look! We were the last ones referencing this node, and we know
        // we want to add it to the free list, so let's do it!
        add(node);
    }
}

FixSizedTask* Stack::pop()
{
    auto head = stackHead.load(std::memory_order_acquire);
    while (head != nullptr) {
        auto prevHead = head;
        auto refs = head->meta.refcnt.load(std::memory_order_relaxed);
        if ((refs & REFS_MASK) == 0 || !head->meta.refcnt.compare_exchange_strong(refs, refs + 1,
                std::memory_order_acquire, std::memory_order_relaxed)) {
            head = stackHead.load(std::memory_order_acquire);
            continue;
        }
        // Good, reference count has been incremented (it wasn't at zero), which means
        // we can read the next and not worry about it changing between now and the time
        // we do the CAS
        auto next = head->meta.next.load(std::memory_order_relaxed);
        if (stackHead.compare_exchange_strong(head, next,
                std::memory_order_acquire, std::memory_order_relaxed)) {
            head->meta.refcnt.fetch_add(-2, std::memory_order_relaxed);
            return head;
        }
        // OK, the head must have changed on us, but we still need to decrease the refcount we
        // increased
        refs = prevHead->meta.refcnt.fetch_add(-1, std::memory_order_acq_rel);
        if (refs == SHOULD_BE_ON_FREELIST + 1) {
            add(prevHead);
        }
    }
    return nullptr;
}

std::string Stack::stats(){
    std::string duh="Stack(";
    int count=0;
    FixSizedTask* pos = stackHead.load(std::memory_order_relaxed);
    while(pos!=nullptr){
        count++;
        if(pos->location()!=FixSizedTask::atFreeList){
            duh+=pos->taskPointer()->stats()+" ";
            pos = pos->meta.next.load(std::memory_order_relaxed);
        }else{
            duh+="<empty> ";
        }
    }
    duh+=")[count="+std::to_string(count)+"]";
    return duh;
}

void PrivateStack::push(FixSizedTask* node)
{
   // node->meta.next.store(stackHead, std::memory_order_relaxed);
    node->meta.next.store(stackHead);
    stackHead=node;
    count++;
}

FixSizedTask* PrivateStack::pop()
{
    // uint8_t tid=Pool::instance().currentThreadIndex();
    // if(tid!=workerid){
    //    println("PrivateStack::pop(): worker id="+std::to_string(workerid)+", current tid="+std::to_string(tid)); 
    // } 
    if(stackHead==nullptr) return nullptr;
    FixSizedTask* next=stackHead->meta.next.load();
    //FixSizedTask* next=stackHead->meta.next.load(std::memory_order_relaxed);
    FixSizedTask* tmp=stackHead;
    stackHead=next;
    count--;
    return tmp;
}

std::string PrivateStack::stats(){
    std::string duh="Stack(";
    int c=0;
    FixSizedTask* pos = stackHead;
    while(pos!=nullptr){
        c++;
        if(pos->location()!=FixSizedTask::atFreeList){
            duh+=pos->taskPointer()->stats()+" ";
            pos = pos->meta.next.load(std::memory_order_relaxed);
        }else{
            duh+="<empty> ";
        }
    }
    duh+=")[count="+std::to_string(c)+"]";
    return duh;
}

 int PrivateStack::size(){ //slow, debugging
    auto pos=stackHead;
    int c=0;
    while(pos!=nullptr){
        c++;
        pos=pos->meta.next.load();
    }
    if(count!=c){
        //奇怪，这里出现不等。pos为nullptr，而事实上这个stack却很大。难道哪里多线程访问了它吗？
        println("c="+std::to_string(c)+", count="+std::to_string(count));
    }
    //assert(count==c);
    return c;
}

void BlockingStack::push(FixSizedTask* node)
{
    std::lock_guard<std::mutex> lk(mtx);
    node->meta.next.store(stackHead);
    stackHead=node;
}

FixSizedTask* BlockingStack::pop()
{
    std::lock_guard<std::mutex> lk(mtx);
    if(stackHead==nullptr) return nullptr;
    FixSizedTask* next=stackHead->meta.next.load();
    FixSizedTask* tmp=stackHead;
    stackHead=next;
    return tmp;
}

std::string BlockingStack::stats(){
    std::string duh="Stack(";
    int count=0;
    FixSizedTask* pos = stackHead;
    while(pos!=nullptr){
        count++;
        if(pos->location()!=FixSizedTask::atFreeList){
            duh+=pos->taskPointer()->stats()+" ";
            pos = pos->meta.next.load(std::memory_order_relaxed);
        }else{
            duh+="<empty> ";
        }
    }
    duh+=")[count="+std::to_string(count)+"]";
    return duh;
}

}