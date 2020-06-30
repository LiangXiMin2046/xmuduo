#ifndef MUDUO_BASE_CONDITION_H
#define MUDUO_BASE_CONDITION_H

#include "/muduoday/day5/muduo/base/Mutex.h"

#include<boost/noncopyable.hpp>
#include<pthread.h>

namespace muduo
{
class Condition : boost::noncopyable
{
public:
	explicit Condition(MutexLock& mutex):mutex_(mutex)
	{
		pthread_cond_init(&pcond_,NULL);
	}

	~Condition()
	{
		pthread_cond_destroy(&pcond_);
	}

	void wait()
	{
		pthread_cond_wait(&pcond_,mutex_.getPthreadMutex());
	}

	bool waitForSeconds(int seconds);
	
	void notify()
	{
		pthread_cond_signal(&pcond_);
	}

	void notifyAll()
	{
		pthread_cond_broadcast(&pcond_);
	}

private:
	MutexLock& mutex_;
	pthread_cond_t pcond_;
};

}

#endif

