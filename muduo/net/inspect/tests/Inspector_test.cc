#include "/xmuduo/muduo/net/inspect/Inspector.h"
#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/EventLoopThread.h"

using namespace muduo;
using namespace muduo::net;

int main(void)
{
	EventLoop loop;
	EventLoopThread t;
	Inspector ins(t.startLoop(),InetAddress(9527),"test");
	loop.loop();
	return 0;
}
