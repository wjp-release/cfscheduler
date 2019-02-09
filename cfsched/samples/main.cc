#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
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

void simple(){
    cfsched::Pool::instance().start();
    cfsched::Pool::instance().emplaceRoot<A>(12);
    cfsched::Pool::instance().emplaceRoot<A>(10);
    cfsched::Pool::instance().emplaceRoot<A>(6);
    cfsched::sleep(500);
}

int main() {
    simple();

    return 0;
}

