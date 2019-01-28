#include "gtest/gtest.h"
#include "utils.h"
#include "stack.h"
#include "task.h"

using namespace cfsched;

class StackTest : public ::testing::Test {
protected:
	StackTest() {}
	virtual ~StackTest() {}
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
};


TEST_F(StackTest, basic) {
    Stack stack;
    Task tasks[500];
    for(int i=0;i<500;i++){
        tasks[i].state=i;
    }
    for(int i=0;i<500;i++){
        stack.push(&tasks[i]);
    }
    for(int i=0;i<500;i++){
        auto t=stack.pop();
        EXPECT_EQ(t->state, 500-i-1);
    }}


TEST_F(StackTest, multithreaded) {
    Stack stack;
    Task tasks[50];
    for(int i=0;i<50;i++){
        tasks[i].state=i;
    }
    for(int tid=0;tid<5;tid++){
        std::thread([tid,&stack,&tasks]{
            for(int i=0;i<10;i++){
                stack.push(&tasks[i+tid*10]);
                cfsched::sleep(10);
            }
        }).detach();
    }
    for(int tid=0;tid<2;tid++){
           std::thread([tid,&stack,&tasks]{
            for(int i=0;i<50;i++){
                Task* bobo=stack.pop();
                std::string duh="thread"+std::to_string(tid);
                if(bobo==nullptr){
                    duh+=" fails to get";
                }else{
                    duh+=" gets "+std::to_string(bobo->state);
                }
                cfsched::println(duh);
                cfsched::sleep(2);
            }
        }).detach();     
    }
    cfsched::sleep(500);
}



