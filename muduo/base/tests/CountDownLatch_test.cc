#include "/muduoday/day5/muduo/base/CountDownLatch.h"
#include "/muduoday/day5/muduo/base/Thread.h"
#include<unistd.h>
#include<stdio.h>
#include<string>
#include "/muduoday/day5/muduo/base/CurrentThread.h"
#include<boost/bind.hpp>
#include<boost/ptr_container/ptr_vector.hpp>
#include<vector>

using namespace muduo;

class test
{
public:
	test(int num):latch(1)
	{
		for(int i = 0; i < num; ++i)
		{
			char name[32];
			snprintf(name,sizeof name,"threadnum %d",i+1);
			//threads.push_back(new muduo::Thread(boost::bind(&test::func,this),muduo::string(name)));
			threads.push_back(new Thread(boost::bind(&test::func,this),string(name)));
		}
		//for_each(threads.begin(),threads.end(),boost::bind(&Thread::start,_1));
		for(int i = 0; i < num; ++i)
			threads[i]->start();	
	}
	void run()
	{
		latch.countDown();
	}
	void  joinAll()
	{
		//for_each(threads.begin(),threads.end(),boost::bind(&Thread::join,_1));
		for(int i = 0; i < threads.size(); ++i)
			threads[i]->join();
	}
private:
	void func()
	{
		latch.wait();
		printf("%s start,tid:%d\n",CurrentThread::name(),CurrentThread::tid());	
		printf("%s end,tid:%d\n",CurrentThread::name(),CurrentThread::tid());			
	}
	CountDownLatch latch;
	std::vector<Thread*> threads;
	//boost::ptr_vector<Thread> threads;
};

int main(void)
{
	test t(5);
	sleep(1);
	t.run();
	t.joinAll();
	printf("over,number of threads:%d\n",Thread::numCreated());
}
