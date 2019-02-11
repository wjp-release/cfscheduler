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
        return cfsched::Task::stats()+"<"+val+">";
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
    B(int x) : x(x){}
    virtual std::string stats() override{
        return cfsched::Task::stats()+"<"+std::to_string(x)+">";
    }
protected:
    virtual void        compute() override{
        if(x>=10) return;
        spawn<B>(x+1);
        spawn<B>(x+1);
        localSync();
        cfsched::println(std::to_string(x)+" computed!");
    }
private:
    int x;
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
    B* b=cfsched::Pool::instance().emplaceRoot<B>(1);
    b->externalSync();
    cfsched::println("b has been externally sync-ed");
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
    emplace_root();

    return 0;
}

