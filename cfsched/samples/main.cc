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

class B : public cfsched::Task{
public:
    B(int lv, int val) :level(lv),value(val){}
    virtual std::string stats() override{
        return "<"+std::to_string(level)+","+std::to_string(value)+">";
    }
protected:
    virtual void        compute() override{
        if(level>=8) goto ret;
        spawn<B>(level+1,value*2);
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
    cfsched::Pool::instance().start();
    std::vector<int> arr;
    for(int i=0;i<100;i++){
        arr.push_back(1);
    }
    int res=cfsched::parallel_sum<10>(arr.data(),arr.size());
    std::cout<<"res="<<res<<std::endl;
}

void sum_i(){
    cfsched::Pool::instance().start();
    std::vector<int> arr;
    for(int i=0;i<100;i++){
        arr.push_back(i);
    }
    int res=cfsched::parallel_sum<10>(arr.data(),arr.size());
    cfsched::println("res="+std::to_string(res));
    //std::cin.get();
}

void large_sum(){
    cfsched::time_point start=cfsched::now();
    cfsched::Pool::instance().start();
    std::vector<int> arr;
    for(int i=0;i<10000;i++){
        arr.push_back(1);
    }
    cfsched::println("Everything prepared, "+std::to_string(cfsched::ms_elapsed_count(start))+" ms elapsed.");
    start=cfsched::now();
    int res=cfsched::parallel_sum<10>(arr.data(),arr.size());
    cfsched::println("res="+std::to_string(res)+", "+std::to_string(cfsched::ms_elapsed_count(start))+" ms elapsed.");
    cfsched::Profiler::instance().print();
}

//debug思路：多加assert，验证sum是不是负数。
//去掉reclaim看看
//目前仍有sum为负数，freelist中途出现nullptr，这两种情况。

int main() {
    large_sum();
    return 0;
}

