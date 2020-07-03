#include "/xmuduo/muduo/base/ThreadPool.h"
#include "/xmuduo/muduo/base/CurrentThread.h"
#include<stdio.h>
#include<unistd.h>
#include<boost/bind.hpp>
#include<string>

void print(const std::string& str)
{
	printf("%s,by %d\n",str.c_str(),muduo::CurrentThread::tid());
	return;
}

int main(void)
{
	muduo::ThreadPool pool("pool");
	pool.start(5);
	for(int i = 0; i < 100;i++)
	{
		char buf[32];
		snprintf(buf,sizeof buf,"task %d",i);
		pool.run(boost::bind(print,std::string(buf)));
		usleep(100000);
	}	 
	return 0;
}
