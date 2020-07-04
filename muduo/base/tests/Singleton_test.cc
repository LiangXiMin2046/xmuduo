#include "/xmuduo/muduo/base/Singleton.h"
#include "/xmuduo/muduo/base/Thread.h"

#include<stdio.h>
#include<string>
#include<boost/bind.hpp>

class Model
{
public:
	void setName(const std::string &name)
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

void changeName()
{
	printf("name in other thread:%s\n",muduo::Singleton<Model>::instance().getName().c_str());
	muduo::Singleton<Model>::instance().setName("new name");
}

int main(void)
{
	muduo::Singleton<Model>::instance().setName("old name");
	printf("name in main thread:%s\n",muduo::Singleton<Model>::instance().getName().c_str());
	muduo::Thread t(changeName);
	t.start();
	t .join();
	printf("name in main thread:%s\n",muduo::Singleton<Model>::instance().getName().c_str());
	return 0;
}
