#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "/xmuduo/muduo/base/Mutex.h"
#include "/xmuduo/muduo/base/Thread.h"
#include "/xmuduo/muduo/base/Timestamp.h"
#include "/xmuduo/muduo/net/Callbacks.h"
#include "/xmuduo/muduo/net/TimerId.h"

namespace muduo
{
namespace net
{
class Channel;
class Poller;
class TimerQueue;

//Reactor,at most one per thread.
class EventLoop : boost::noncopyable
{
public:
	typedef boost::function<void()> Functor;
	EventLoop();
	~EventLoop();
	
	void loop();
	void quit();
	//
	//Time when poll returns,usually means data arrival
	//
	Timestamp pollReturnTime() const {return pollReturnTime_;}

	//
	//wakes up loop and run the cb
	//can be called in other thread
	//
	void runInLoop(const Functor& cb);
	
	void queueInLoop(const Functor& cb);
	
	TimerId runAt(const Timestamp& time,const TimerCallback& cb);

	TimerId runAfter(double delay,const TimerCallback& cb);
	
	TimerId runEvery(double interval,const TimerCallback& cb);

	void cancel(TimerId timerId);
	void wakeup();
	void updateChannel(Channel* channel);
	void removeChannel(Channel* channel);
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
	void handleRead(); //for wake up
	void doPendingFunctors();
	void printActiveChannels() const;
	
	typedef std::vector<Channel*>ChannelList;

	bool looping_;
	bool quit_;
	bool eventHandling_;
	bool callingPendingFunctors_;
	const pid_t threadId_;
	Timestamp pollReturnTime_;
	boost::scoped_ptr<Poller> poller_;
	boost::scoped_ptr<TimerQueue> timerQueue_;
	int wakeupFd_;
	boost::scoped_ptr<Channel> wakeupChannel_;
	ChannelList activeChannels_;
	Channel* currentActiveChannel_;
	MutexLock mutex_;
	std::vector<Functor> pendingFunctors_;
};

}
}
#endif
