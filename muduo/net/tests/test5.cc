#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/base/Thread.h"

#include <boost/bind.hpp>
#include <stdio.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

EventLoop* g_loop;
int cnt = 0;

void print(const char* msg)
{
	printf("msg %s,%s\n",Timestamp::now().toString().c_str(),msg);
	if(++cnt == 20)
		g_loop -> quit();
}

void cancel(TimerId timer)
{
	g_loop -> cancel(timer);
	printf("cancelled at %s \n",Timestamp::now().toString().c_str());
}

int main()
{
	EventLoop loop;
	g_loop = &loop;
	g_loop -> runAfter(1.0,boost::bind(print,"once1"));
	g_loop -> runAfter(1.5,boost::bind(print,"once1.5"));
	g_loop -> runAfter(2.0,boost::bind(print,"once2.0"));
	g_loop -> runAfter(3.5,boost::bind(print,"once3.5"));
	TimerId t45 = g_loop -> runAfter(4.5,boost::bind(print,"once4.5"));
	g_loop -> runAfter(4.2,boost::bind(cancel,t45));	
	g_loop -> runAfter(4.8,boost::bind(cancel,t45));
	g_loop -> runEvery(2,boost::bind(print,"every2"));
	TimerId t3 = g_loop -> runEvery(3,boost::bind(print,"every 3"));
	g_loop -> runAfter(9.0,boost::bind(cancel,t3));
	g_loop -> loop();
	printf("exit");
	return 0;
}
