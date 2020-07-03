#ifndef MUDUO_BASE_THREADPOOL_H
#define MUDUO_BASE_THREADPOOL_H

#include "/xmuduo/muduo/base/Thread.h"
#include "/xmuduo/muduo/base/Condition.h"
#include "/xmuduo/muduo/base/Mutex.h"
#include "/xmuduo/muduo/base/Types.h"

#include<deque>
#include<boost/ptr_container/ptr_vector.hpp>
#include<boost/function.hpp>
#include<boost/noncopyable.hpp>

namespace muduo
{
class ThreadPool : boost::noncopyable
{
public:
	typedef boost::function<void()>	Task;
	explicit ThreadPool(const string& name=string());
	~ThreadPool();

	void start(int numThreads);
	void stop();

	void run(const Task& f);
private:
	void runInThread();
	Task take();

	MutexLock mutex_;
	Condition cond_;
	string name_;
	boost::ptr_vector<muduo::Thread> threads_;
	std::deque<Task> queue_;
	bool running_;
};
}
#endif
