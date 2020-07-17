#include "/xmuduo/muduo/net/Poller.h"
#include "/xmuduo/muduo/net/poller/PollPoller.h"
//#include "/xmuduo/muduo/net/Poller/EpollPoller.h"

#include <stdlib.h>

using namespace muduo::net;

Poller* Poller::newDefaultPoller(EventLoop* loop)
{
	return new PollPoller(loop);
}
