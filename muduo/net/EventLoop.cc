#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/base/Logging.h"
#include "/xmuduo/muduo/net/Poller.h"
#include "/xmuduo/muduo/net/Channel.h"
#include "/xmuduo/muduo/net/TimerQueue.h"

//#include <poll.h>

using namespace muduo;
using namespace muduo::net;

namespace
{
__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
	return t_loopInThisThread;
}

EventLoop::EventLoop()
	:	
		looping_(false),
		quit_(false),
		eventHandling_(false),
		poller_(Poller::newDefaultPoller(this)),
		threadId_(CurrentThread::tid()),
		timerQueue_(new TimerQueue(this)),
		currentActiveChannel_(NULL)
{
	LOG_TRACE<< "EventLoop created" << this << "in thread" << threadId_;
	if(t_loopInThisThread)
	{
		LOG_FATAL<<"Another EventLoop" << t_loopInThisThread
				<< "exsits in this thread" << threadId_;	
	}
	else
	{
		t_loopInThisThread = this;
	}
}

EventLoop::~EventLoop()
{
	t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
	assert(!looping_);
	assertInLoopThread();
	looping_ = true;
	LOG_TRACE << "EventLoop" << this << "start looping";

	//::poll(NULL,0,5*1000);
	while(!quit_)
	{
		activeChannels_.clear();
		pollReturnTime_ = poller_ -> poll(kPollTimeMs,&activeChannels_);
		if(Logger::logLevel() <= Logger::TRACE)
		{
			printActiveChannels();
		}

		eventHandling_ = true;
		for(ChannelList::iterator it = activeChannels_.begin();
				it != activeChannels_.end(); ++it)
		{
			currentActiveChannel_ = *it;
			currentActiveChannel_ -> handleEvent(pollReturnTime_);
		}
		currentActiveChannel_ = NULL;
		eventHandling_ = false;
	}	
	LOG_TRACE << "EventLoop" << this << "stop looping";
	looping_ = false;
}

void EventLoop::quit()
{
	quit_ = true;
	if(!isInLoopThread())
	{
		
	}
}

TimerId EventLoop::runAt(const Timestamp& time,const TimerCallback& cb)
{
	return timerQueue_ -> addTimer(cb,time,0.0);
}

TimerId EventLoop::runAfter(double delay,const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(),delay));
	return runAt(time,cb);
}

TimerId EventLoop::runEvery(double interval,const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(),interval));
	return timerQueue_ -> addTimer(cb,time,interval);
}

void EventLoop::cancel(TimerId timerId)
{
	return timerQueue_ ->cancel(timerId);
}

void EventLoop::updateChannel(Channel* channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_ -> updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
	assert(channel -> ownerLoop() == this);
	assertInLoopThread();
	if(eventHandling_)
	{
		assert(currentActiveChannel_ == channel ||
				std::find(activeChannels_.begin(),activeChannels_.end(),channel) 
					== activeChannels_.end());
	}
	poller_ -> removeChannel(channel);
}
void EventLoop::abortNotInLoopThread()
{
	LOG_FATAL << "EventLoop::abortNotIntLoopThread-EventLoop"<<this<<" was created in threadId_ = " << threadId_
				<< ",current thread id = " << CurrentThread::tid();
}

void EventLoop::printActiveChannels() const
{
	for(ChannelList::const_iterator it = activeChannels_.begin();
			it != activeChannels_.end(); ++it)
	{
		const Channel* ch = *it;
		LOG_TRACE << "{ " << ch -> reventsToString() << " } ";
	}
}
