#include "gtest/gtest.h"
#include "utils.h"
#include "stack.h"
#include "task_physical.h"

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
    FixSizedTask tasks[500];
    for(int i=0;i<500;i++){
        tasks[i].meta.state=i;
    }
    for(int i=0;i<500;i++){
        stack.push(&tasks[i]);
    }
    for(int i=0;i<500;i++){
        auto t=stack.pop();
        EXPECT_EQ(t->meta.state, 500-i-1);
    }
}


TEST_F(StackTest, private) {
    PrivateStack stack;
    FixSizedTask tasks[500];
    for(int i=0;i<500;i++){
        tasks[i].meta.state=i;
    }
    for(int i=0;i<500;i++){
        stack.push(&tasks[i]);
    }
    for(int i=0;i<500;i++){
        auto t=stack.pop();
        EXPECT_EQ(t->meta.state, 500-i-1);
    }
}

TEST_F(StackTest, multithreaded) {
    Stack stack;
    FixSizedTask tasks[50];
    for(int i=0;i<50;i++){
        tasks[i].meta.state=i;
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
                FixSizedTask* bobo=stack.pop();
                std::string duh="thread"+std::to_string(tid);
                if(bobo==nullptr){
                    duh+=" fails to get";
                }else{
                    duh+=" gets "+std::to_string(bobo->meta.state)+", refcnt="+std::to_string(bobo->meta.refcnt);
                }
                cfsched::println(duh);
                cfsched::sleep(2);
            }
        }).detach();     
    }
    cfsched::sleep(500);
}



