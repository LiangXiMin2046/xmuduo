#include "/xmuduo/muduo/base/ThreadLocalSingleton.h"
#include "/xmuduo/muduo/base/Thread.h"

#include<stdio.h>
#include<boost/noncopyable.hpp>
#include<boost/bind.hpp>

class Test : boost::noncopyable
{
public:
	Test() : name_("original name")
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

void func(const std::string& newname,int i)
{
	printf("in thread %d,befeore change:name:%s,instance location:%p\n",
			i,muduo::ThreadLocalSingleton<Test>::instance().getName().c_str(),
			&muduo::ThreadLocalSingleton<Test>::instance());
	muduo::ThreadLocalSingleton<Test>::instance().setName(newname);
	printf("in thread %d,after change:name:%s,instance location:%p\n",
			i,muduo::ThreadLocalSingleton<Test>::instance().getName().c_str(),
			&muduo::ThreadLocalSingleton<Test>::instance());	
}

int main(void)
{
	printf("in main thread,befeore change:name:%s,instance location:%p\n",
			muduo::ThreadLocalSingleton<Test>::instance().getName().c_str(),
			&muduo::ThreadLocalSingleton<Test>::instance());
	muduo::ThreadLocalSingleton<Test>::instance().setName("main thread");
	muduo::Thread t1(boost::bind(func,"thread 1",1));
	muduo::Thread t2(boost::bind(func,"thread 2",2));
	t1.start();
	t2.start();
	t2.join();
	t2.join();
	printf("in main thread,after change:name:%s,instance location:%p\n",
			muduo::ThreadLocalSingleton<Test>::instance().getName().c_str(),
			&muduo::ThreadLocalSingleton<Test>::instance());
	return 0;
}


