#ifndef MUDUO_NET_EVENTLOOPTHREADPOOL_H
#define MUDUO_NET_EVENTLOOPTHREADPOOL_H

#include "/xmuduo/muduo/base/Condition.h"
#include "/xmuduo/muduo/base/Mutex.h"

#include <vector>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace muduo
{
namespace net
{
class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : boost::noncopyable
{
public:
	typedef boost::function<void(EventLoop*)> ThreadInitCallback;
	
	EventLoopThreadPool(EventLoop* baseLoop);
	~EventLoopThreadPool();
	void setThreadNum(int numThreads)
	{  numThreads_ = numThreads;  }
	void start(const ThreadInitCallback& cb = ThreadInitCallback());
	EventLoop* getNextLoop();
	
private:
	EventLoop* baseLoop_; //same EventLoop as acceptor
	bool started_;
	int numThreads_;
	int next_;  //index of the chosen EventLoop when new connection arrives
	boost::ptr_vector<EventLoopThread> threads_;
	std::vector<EventLoop*> loops_;
};

}
}

#endif
