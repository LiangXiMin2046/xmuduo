#ifndef MUDUO_NET_POLLER_POLLPOLLER_H
#define MUDUO_NET_POLLER_POLLPOLLER_H

#include "/xmuduo/muduo/net/Poller.h"

#include <map>
#include <vector>

struct pollfd;

namespace muduo
{
namespace net
{
//IO Multiplexing with poll(2)
class PollPoller : public Poller
{
	public:
		PollPoller(EventLoop* loop);
		
		virtual ~PollPoller();
		
		virtual Timestamp poll(int timeoutMs,ChannelList* activeChannels);
		virtual void updateChannel(Channel* channel);
		virtual void removeChannel(Channel* channel);
	private:
		void fillActiveChannels(int numEvents,ChannelList* activeChannels) const;
		typedef std::vector<struct pollfd> PollFdList;
		typedef std::map<int,Channel*> ChannelMap; //key:fd,value:pointer of Channel
		PollFdList pollfds_;
		ChannelMap channels_;	
};

}
}

#endif
