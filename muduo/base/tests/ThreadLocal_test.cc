#include "/xmuduo/muduo/base/ThreadLocal.h"
#include "/xmuduo/muduo/base/CurrentThread.h"
#include "/xmuduo/muduo/base/Thread.h"

#include<unistd.h>
#include<string>
#include<stdio.h>

class Model
{
public:
	Model()
	  :name_("original name")
	{
	}
	
	void setName(const std::string& name)
	{
		name_ = name;	
	}

	std::string& getName()
	{
		return name_;
	}

private:
	std::string name_;
};

muduo::ThreadLocal<Model> m1;
muduo::ThreadLocal<Model> m2;

void func()
{
	printf("in another thread,tid:%d,m1's name:%s,m2's name:%s\n",
		muduo::CurrentThread::tid(),m1.value().getName().c_str(),m2.value().getName().c_str());
	m1.value().setName("thread name m1");
	m2.value().setName("thread name m2");
	printf("name changed in another thread,tid:%d,m1's name:%s,m2's name:%s\n",
		muduo::CurrentThread::tid(),m1.value().getName().c_str(),m2.value().getName().c_str());
	sleep(1);	
}

int main(void)
{
	printf("in main thread,tid:%d,m1's name:%s,m2's name:%s\n",
		muduo::CurrentThread::tid(),m1.value().getName().c_str(),m2.value().getName().c_str());
	muduo::Thread t1(func,"thread1");
	t1.start();
	t1.join();
	printf("in main thread,tid:%d,m1's name:%s,m2's name:%s\n",
		muduo::CurrentThread::tid(),m1.value().getName().c_str(),m2.value().getName().c_str());
	return 0;
}
