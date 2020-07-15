#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <boost/noncopyable.hpp>

#include "/xmuduo/muduo/base/CurrentThread.h"
#include "/xmuduo/muduo/base/Thread.h"

namespace muduo
{
namespace net
{
//Reactor,at most one per thread.
class EventLoop : boost::noncopyable
{
public:
	EventLoop();
	~EventLoop();
	
	void loop();
	
	void assertInLoopThread()
	{
		if(!isInLoopThread())
		{
			abortNotInLoopThread();
		}
	}
	bool isInLoopThread() const {return threadId_ == CurrentThread::tid();}

	static EventLoop* getEventLoopOfCurrentThread();
private:
	void abortNotInLoopThread();
	bool looping_;
	const pid_t threadId_;
};

}
}
#endif
