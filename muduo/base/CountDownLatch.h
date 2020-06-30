#ifndef MUDUO_BASE_COUNTDOWNLATCH_H
#define MUDUO_BASE_COUNTDOWNLATCH_H

#include "/xmuduo/muduo/base/Mutex.h"
#include "/xmuduo/muduo/base/Condition.h"

#include<boost/noncopyable.hpp>

namespace muduo
{
class CountDownLatch : boost::noncopyable
{
public:
	explicit CountDownLatch(int count);

	void wait();
	
	void countDown();

	int getCount() const;

private:
	mutable MutexLock mutex_;
	Condition condition_;
	int count_;	
};
}

#endif
