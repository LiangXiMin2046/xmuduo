#include "/xmuduo/muduo/base/Thread.h"
#include "/xmuduo/muduo/base/CurrentThread.h"

#include<stdio.h>
#include<boost/bind.hpp>
#include<string>
void threadFunc()
{
	printf("tid:%d\n",muduo::CurrentThread::tid());
}

void threadFunc2(int x)
{
	printf("tid:%d,x:%d\n",muduo::CurrentThread::tid(),x);
}

class Foo{
public:
	Foo(double x):x_(x){}
	void memberFunc()
	{
		printf("tid:%d,Foo::x=%f\n",muduo::CurrentThread::tid(),x_);
	}
	
	void memberFunc2(const std::string& text)
	{
		printf("tid:%d,text:%s\n",muduo::CurrentThread::tid(),text.c_str());
	}
private:
	double x_;
};

int main(void)
{
	muduo::Thread t1(threadFunc);
	t1.start();
	t1.join();
	muduo::Thread t2(boost::bind(threadFunc2,12),"boost bind");
	t2.start();
	t2.join();
	Foo foo(11.21);
	muduo::Thread t3(boost::bind(&Foo::memberFunc,&foo));
	t3.start();
	t3.join();
	muduo::Thread t4(boost::bind(&Foo::memberFunc2,boost::ref(foo),std::string("liang")));
	t4.start();
	t4.join();
	return 0;
}
