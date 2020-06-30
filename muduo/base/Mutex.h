#ifndef MUDUO_BASE_MUTEX_H
#define MUDUO_BASE_MUTEX_H

#include "/muduoday/day5/muduo/base/CurrentThread.h"
#include<pthread.h>
#include<boost/noncopyable.hpp>
#include<assert.h>

namespace muduo
{
class MutexLock : boost::noncopyable
{
public:
	MutexLock():holder_(0)
	{
		int ret = pthread_mutex_init(&mutex_,NULL);
		assert(ret == 0);(void) ret;
	}
	
	~MutexLock()
	{
		assert(holder_ == 0);
		int ret = pthread_mutex_destroy(&mutex_);
		assert(ret == 0);(void)ret;
	}
	
	bool isLockedByThisThread()
	{
		return holder_ == CurrentThread::tid();
	}
	
	bool assertLocked()
	{
		assert(isLockedByThisThread());
	}
	
	void lock()
	{
		pthread_mutex_lock(&mutex_);
		holder_ = CurrentThread::tid();
	}
	
	void unlock()
	{
		holder_ = 0;
		pthread_mutex_unlock(&mutex_);
	}

	pthread_mutex_t* getPthreadMutex()
	{
		return &mutex_;
	}
private:
	pthread_mutex_t mutex_;
	pid_t holder_;
};

class MutexLockGuard : boost::noncopyable
{
public:
	explicit MutexLockGuard(MutexLock &mutex)
	: mutex_(mutex)
	{
		mutex_.lock();
	}

	~MutexLockGuard()
	{
		mutex_.unlock();
	}
private:
	MutexLock &mutex_;
};

}

#define MutexLockGuard(x) error "missing guard object name"
#endif
