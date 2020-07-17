#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/base/CurrentThread.h"
#include "/xmuduo/muduo/base/Thread.h"

#include <stdio.h>
#include <unistd.h>

muduo::net::EventLoop* g_ptr(new muduo::net::EventLoop());
void threadFunc()
{
	printf("pid:%d,current tid:%d\n",getpid(),muduo::CurrentThread::tid());
	g_ptr -> loop();
}

int main()
{
	printf("pid:%d,current tid:%d\n",getpid(),muduo::CurrentThread::tid());
	g_ptr -> loop();
	muduo::Thread t(threadFunc);
	t.start();
	t.join();
	return 0;
}
