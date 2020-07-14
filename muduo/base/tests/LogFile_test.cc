#include "/xmuduo/muduo/base/Logging.h"
#include "/xmuduo/muduo/base/LogFile.h"
#include <stdio.h>

boost::scoped_ptr<muduo::LogFile> g_logfile;

void outputFunc(const char* msg,int len)
{
	g_logfile -> append(msg,len);
} 

void flushFunc()
{
	g_logfile -> flush();
}

int main(int argc,char** argv)
{
	char name[256];
	strncpy(name,argv[0],256);
	printf("%s\n",name);
	g_logfile.reset(new muduo::LogFile(::basename(name),150 * 1024));
	muduo::Logger::setOutput(outputFunc);
	muduo::Logger::setFlush(flushFunc);
	muduo::string str = "1234567890qwertyuiopasdfghjklzxcvbnm";
	LOG_INFO<<"test"<<100;
	for(int i = -10; i < 10;i++)
	{
		LOG_INFO<<str<<i;
		usleep(1000);
	}
	return 0;
}
