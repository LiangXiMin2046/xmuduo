#ifndef MUDUO_BASE_BOUNDBLOCKINGQUEUE_H
#define MUDUO_BASE_BOUNDBLOCKINGQUEUE_H

#include "/xmuduo/muduo/base/Mutex.h"
#include "/xmuduo/muduo/base/Condition.h"

#include<deque>
#include<boost/noncopyable.hpp>
#include<boost/circular_buffer.hpp>
#include<assert.h>

namespace muduo
{
template<typename T>
class BoundedBlockingQueue : boost::noncopyable
{
public:
	BoundedBlockingQueue(int maxSize)
	  : mutex_(),notEmpty_(mutex_),notFull_(mutex_),queue_(maxSize)
	{
	}
	
	void put(const T& x)
	{
		MutexLockGuard lock(mutex_);
		while(queue_.full())
		{
			notFull_.wait();
		}	
		assert(!queue_.full());
		queue_.push_back(x);
		notEmpty_.notify();
	}

	T take()
	{
		MutexLockGuard lock(mutex_);
		while(queue_.empty())
		{		
			notEmpty_.wait();
		}
		assert(!queue_.empty());
		T front(queue_.front());
		queue_.pop_front();
		notFull_.notify();
		return front;
	}

	bool empty() const
	{
		MutexLockGuard lock(mutex_);
		return queue_.empty();	
	}
	
	bool full() const
	{
		MutexLockGuard lock(mutex_);
		return queue_.full();
	}
	
	bool size() const
	{
		MutexLockGuard lock(mutex_);
		return queue_.size();
	}

	bool capacity() const
	{
		MutexLockGuard lock(mutex_);
		return queue_.capacity();
	}
private:
	MutexLock mutex_;
	Condition notEmpty_;
	Condition notFull_;
	boost::circular_buffer<T> queue_;
};
}
#endif
