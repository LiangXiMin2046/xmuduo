#ifndef MUDUO_NET_TIMEQUEUE_H
#define MUDUO_NET_TIMEQUEUE_H

#include <set>
#include <vector>

#include <boost/noncopyable.hpp>

#include "/xmuduo/muduo/base/Timestamp.h"
#include "/xmuduo/muduo/base/Mutex.h"
#include "/xmuduo/muduo/net/Callbacks.h"
#include "/xmuduo/muduo/net/Channel.h"

namespace muduo
{
namespace net
{
class EventLoop;
class Timer;
class TimerId;

class TimerQueue : boost::noncopyable
{
public:
	TimerQueue(EventLoop* loop);
	~TimerQueue();
	
	TimerId addTimer(const TimerCallback& cb,
				     Timestamp when,
					 double interval);
	
	void cancel(TimerId timerId);
private:
	typedef std::pair<Timestamp,Timer*> Entry;
	typedef std::set<Entry> TimerList;
	typedef std::pair<Timer*,int64_t> ActiveTimer;
	typedef std::set<ActiveTimer> ActiveTimerSet;

	void addTimerInLoop(Timer* timer);
	void cancelInLoop(TimerId timerId);
	//called when timerfd alarms
	void handleRead();
	std::vector<Entry> getExpired(Timestamp now);
	void reset(const std::vector<Entry>& expired,Timestamp now);
	
	bool insert(Timer* timer);

	EventLoop* loop_;
	const int timerfd_;
	Channel timerfdChannel_;
	TimerList timers_; //timers sorted by time

	ActiveTimerSet activeTimers_;
	bool callingExpiredTimers_;
	ActiveTimerSet cancelingTimers_;
};

}
}

#endif
