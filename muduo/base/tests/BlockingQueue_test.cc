#include "/xmuduo/muduo/base/Thread.h"
#include "/xmuduo/muduo/base/BlockingQueue.h"
#include "/xmuduo/muduo/base/CurrentThread.h"
#include "/xmuduo/muduo/base/CountDownLatch.h"

#include <boost/ptr_container/ptr_vector.hpp>
#include<boost/bind.hpp>
#include<stdio.h>
#include<unistd.h>

class Model
{
public:
	Model(int numThread):
	  queue_(),latch_(numThread),threads_(numThread)
	    {
			for(int i = 0; i < numThread; i++)
			{
				char name[32];
				snprintf(name,sizeof name,"threadorder %d",i);
				threads_.push_back(new muduo::Thread(boost::bind(&Model::func_,this),muduo::string(name)));
			}
			for_each(threads_.begin(),threads_.end(),boost::bind(&muduo::Thread::start,_1));
			
		}
	
	void start(int num)
	{
		latch_.wait();
		for(int i = 0; i < num; ++i)
		{
			queue_.put(i);
			printf("product %d has provided\n",i);
			usleep(500);
		}
	}

	void join()
	{
		const int end = -1;
		for(int i = 0; i < threads_.size(); ++i)
			queue_.put(end);
		for_each(threads_.begin(),threads_.end(),bind(&muduo::Thread::join,_1));	
	}

private:	
	void func_()
	{
		latch_.countDown();
		bool running = true;
		int result;
		while(running)
		{
			result = queue_.take();
			if(result >= 0)
				printf("got product %d,tid:%d,name:%s\n",result,muduo::CurrentThread::tid(),muduo::CurrentThread::name());
			else
			{
				printf("thread is finished,tid:%d\n",muduo::CurrentThread::tid());
				running = false;
			}
			usleep(1000);			
		}		
	}

	muduo::BlockingQueue<int> queue_;
	muduo::CountDownLatch latch_;
	boost::ptr_vector<muduo::Thread> threads_;	
};

int main(void)
{
	Model m(5);
	m.start(100);
	m.join();
	printf("simulation over,number of threads:%d\n",muduo::Thread::numCreated());
}
