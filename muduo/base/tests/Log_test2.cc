#include "/xmuduo/muduo/base/Logging.h"

#include<errno.h>
#include<stdio.h>

using namespace muduo;

FILE* g_file;

void fileOutput(const char* msg,int len)
{
	fwrite(msg,1,len,g_file);
}

void fileFlush()
{
	fflush(g_file);
}

int main(void)
{
	g_file = fopen("/tmp/muduo_log","ae");
	Logger::setOutput(fileOutput);
	Logger::setFlush(fileFlush);
	LOG_TRACE<< "trace...";
	LOG_DEBUG<< "debug...";
	LOG_INFO<< "info...";
	LOG_WARN<< "warn...";
	LOG_ERROR<< "error...";
	errno = 13;
	LOG_SYSERR<< "syserr...";
	fclose(g_file);
	return 0;
}
