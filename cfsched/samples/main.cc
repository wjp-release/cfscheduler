#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <vector>
#include "cfsched.h"

class A : public cfsched::Task{
public:
    A(int x){
        std::stringstream ss;
        ss<<"A"<<x;
        val=ss.str();
    }
    virtual std::string stats() override{
        return "Task<"+val+">";
    }
protected:
    virtual void        compute() override{
        cfsched::println(val+" computed!");
    }
private:
    std::string val;
};

/*
 spawnPrivate：导致pendingcnt偶尔不为2，这是为何？


*/ 

class B : public cfsched::Task{
public:
    B(int lv, int val) :level(lv),value(val){}
    virtual std::string stats() override{
        return "<"+std::to_string(level)+","+std::to_string(value)+">";
    }
protected:
    virtual void        compute() override{
        if(level>=8) goto ret;
        //assert(cfsched::FixSizedTask::getFixSizedTaskPointer(this)->meta.pendingcnt==0);
        spawnPrivate<B>(level+1,value*2);
        spawnPrivate<B>(level+1,value*2+1);
        cfsched::println(cfsched::Pool::instance().who() +stats()+"'s pendingcnt="+std::to_string(cfsched::FixSizedTask::getFixSizedTaskPointer(this)->meta.pendingcnt));
        //cfsched::sleep(1000);
        localSync();
ret:    cfsched::println(cfsched::Pool::instance().who()+stats()+" computed!");
    }
private:
    // <0,0>  <1,0~1> <2,0~3> <3,0~7> .. <n, 0~2^n-1>
    int level; 
    int value; 
};
void start(){
    cfsched::Pool::instance().start();
    cfsched::sleep(6000);
}

void emplace_root(){
    cfsched::Pool::instance().start();
    cfsched::Pool::instance().emplaceRoot<A>(12);
    cfsched::Pool::instance().emplaceRoot<A>(10);
    cfsched::Pool::instance().emplaceRoot<A>(6);
    cfsched::sleep(500);
}

void spawn_sync(){
    cfsched::Pool::instance().start();
    B* b=cfsched::Pool::instance().emplaceRoot<B>(0,0);
    b->externalSync();
    cfsched::println("b has been externally synchronised");
}

void sum_1(){
    std::vector<int> arr;
    for(int i=0;i<100;i++){
        arr.push_back(1);
    }
    cfsched::parallel_sum<10>(arr.data(),arr.size());
}

void sum_i(){
    std::vector<int> arr;
    for(int i=0;i<100;i++){
        arr.push_back(i);
    }
    cfsched::parallel_sum<10>(arr.data(),arr.size());
}

int main() {
    spawn_sync();

    return 0;
}

