#include "gtest/gtest.h"
#include "task_logical.h"
#include <iostream>

using namespace cfsched;

class TaskTest : public ::testing::Test {
protected:
	TaskTest() {}
	virtual ~TaskTest() {}
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
};

class A : public Task{
public:
	A(int x) : a(x){}
	int a=0;
	virtual void compute() override{
		std::cout<<"a="<<a<<std::endl;
	}
};

TEST_F(TaskTest, layout) {
	RootTask t1;
	RootTask t2;
	FixSizedTask t3;
	EXPECT_EQ(sizeof(t1),sizeof(t3)+sizeof(std::mutex)+sizeof(std::condition_variable));

	A* a1=t1.emplaceTask<A>(1);
	A* a2=t2.emplaceTask<A>(2);
	A* a3=t3.emplaceTask<A>(3);
	a1->execute();
	a2->execute();
	a3->execute();

	FixSizedTask* a3_shadow = FixSizedTask::getFixSizedTaskPointer(a3);
	EXPECT_EQ(a3_shadow, (FixSizedTask*)&t3);
	auto a3_meta = FixSizedTask::getMetaPointer(a3);
	EXPECT_EQ((FixSizedTask*)a3_meta, a3_shadow);
   	EXPECT_EQ(a3_shadow->taskPointer<A>(), a3);

	FixSizedTask* a1_shadow = FixSizedTask::getFixSizedTaskPointer(a1);
	EXPECT_EQ(a1_shadow, (FixSizedTask*)&t1);
	auto a1_meta = FixSizedTask::getMetaPointer(a1);
	EXPECT_EQ((FixSizedTask*)a1_meta, a1_shadow);
   	EXPECT_EQ(a1_shadow->taskPointer<A>(), a1);

	RootTask* a2_shadow = RootTask::getRootTaskPointer(a2);
	EXPECT_EQ(a2_shadow, &t2);
   	EXPECT_EQ(a2_shadow->taskPointer<A>(), a2);
}



