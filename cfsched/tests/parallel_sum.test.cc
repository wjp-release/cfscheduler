#include "gtest/gtest.h"
#include "cfsched.h"

using namespace cfsched;

class ParallelSumTest : public ::testing::Test {
protected:
	ParallelSumTest() {
        cfsched::Pool::instance().start();
    }
	virtual ~ParallelSumTest() {}
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
};


TEST_F(ParallelSumTest, sum_of_1) {
    std::vector<int> arr;
    for(int i=0;i<100;i++){
        arr.push_back(1);
    }
    int res=cfsched::parallel_sum<10>(arr.data(),arr.size());
    EXPECT_EQ(res,100);
}


TEST_F(ParallelSumTest, sum_of_i) {
    std::vector<int> arr;
    for(int i=0;i<100;i++){
        arr.push_back(i);
    }
    int res=cfsched::parallel_sum<10>(arr.data(),arr.size());
    EXPECT_EQ(res,4950);   
}




