#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/base/CurrentThread.h"
#include "/xmuduo/muduo/base/Thread.h"

#include <stdio.h>
#include <unistd.h>

void threadFunc()
{
	printf("pid:%d,current tid:%d\n",getpid(),muduo::CurrentThread::tid());
	muduo::net::EventLoop loop;
	loop.loop();
}

int main()
{
	printf("pid:%d,current tid:%d\n",getpid(),muduo::CurrentThread::tid());
	muduo::net::EventLoop loop;
	loop.loop();
	muduo::Thread t(threadFunc);
	t.start();
	t.join();
	return 0;
}
