#ifndef MUDUO_NET_POLLER_H
#define MUDUO_NET_POLLER_H
#include <vector>
#include <boost/noncopyable.hpp>

#include "/xmuduo/muduo/base/Timestamp.h"
#include "/xmuduo/muduo/net/EventLoop.h"

namespace muduo
{
namespace net
{
class Channel;

//class for IO multuplexing
class Poller : boost::noncopyable
{
public:
	typedef std::vector<Channel*> ChannelList;
	
	Poller(EventLoop* loop);

	virtual ~Poller();

	virtual Timestamp poll(int timeoutMs,ChannelList* activeChannels) = 0;

	//changes the intrested I/O event
	virtual void updateChannel(Channel* channel) = 0;
	
	//remove the channel
	virtual void removeChannel(Channel* channel) = 0;

	static Poller* newDefaultPoller(EventLoop* loop);

	void assertInLoopThread()
	{
		ownerLoop_ -> assertInLoopThread();
	}
private:
	EventLoop* ownerLoop_; //the eventloop poller belongs to
};

}
}

#endif
