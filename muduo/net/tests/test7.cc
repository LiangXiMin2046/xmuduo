#include "/xmuduo/muduo/net/EventLoopThread.h"
#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/base/CurrentThread.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <boost/bind.hpp>

using namespace muduo::net;
using namespace muduo;

void printInfo()
{
	printf("current pid = %d,tid = %d\n",getpid(),CurrentThread::tid());
}
 
int main(void)
{
	printf("in main, pid = %d,tid = %d\n",getpid(),CurrentThread::tid());
	EventLoopThread thread;
	EventLoop* loop = thread.startLoop();
	loop -> runInLoop(printInfo);
	//sleep(1);
	loop -> runAfter(2,printInfo);
	sleep(3);
	loop -> quit();
	printf("exit\n");
	return 0;
}
