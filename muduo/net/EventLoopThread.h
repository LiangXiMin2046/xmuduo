#ifndef MUDUO_NET_EVENTLOOPTHREAD_H
#define MUDUO_NET_EVENTLOOPTHREAD_H

#include "/xmuduo/muduo/base/Thread.h"
#include "/xmuduo/muduo/base/Mutex.h"
#include "/xmuduo/muduo/base/Condition.h"

#include <boost/noncopyable.hpp>

namespace muduo
{
namespace net
{
class EventLoop;

class EventLoopThread : boost::noncopyable
{
public:
	typedef boost::function<void(EventLoop*)> ThreadInitCallback;
	
	EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());
	~EventLoopThread();
	EventLoop* startLoop(); //start thread and make thread a IO thread

private:
	void threadFunc();
	
	EventLoop* loop_;
	bool exiting_;
	Thread thread_;
	MutexLock mutex_;
	Condition cond_;
	ThreadInitCallback callback_;	
};

}
}

#endif
