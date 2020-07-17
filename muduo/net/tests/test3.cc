#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/base/CurrentThread.h"
#include "/xmuduo/muduo/base/Thread.h"

#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("pid:%d,current tid:%d\n",getpid(),muduo::CurrentThread::tid());
	muduo::net::EventLoop loop1,loop2;
	loop1.loop();
	loop2.loop();
	return 0;
}
