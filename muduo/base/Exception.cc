#include "/xmuduo/muduo/base/Exception.h"

#include<cxxabi.h>
#include<execinfo.h>
#include<stdlib.h>
#include<stdio.h>

using namespace muduo;

Exception::Exception(const char *msg):message_(msg){
	fillStackTrace();
}

Exception::Exception(const string &msg):message_(msg){
	fillStackTrace();
}

Exception::~Exception() throw(){}

const char* Exception::what() const throw()
{
	return message_.c_str();
}

const char* Exception::stackTrace() const throw()
{
	return stack_.c_str();
}

void Exception::fillStackTrace()
{
	int len = 200;
	void *buf[len];
	int nptrs = backtrace(buf,len);
	char **strings = backtrace_symbols(buf,nptrs);
	if(strings)
	{
		for(int i = 0; i < nptrs; ++i)
		{
			stack_.append(demangle(strings[i]));
			stack_.push_back('\n');
		}		
		free(strings);
	}	
}

string Exception::demangle(const char *symbol)
{
	size_t size;
	int status;
	char tem[128];
	char *demangles;
	if(1 == sscanf(symbol,"%*[^(]%*[^_]%127[^)+]",tem))
	{
		if(NULL != (demangles = abi::__cxa_demangle(tem,NULL,&size,&status)))
		{
			string result(demangles);
			free(demangles);
			return result;
		}
	}
	if(1 == sscanf(symbol,"%127s",tem))
	{
		return tem;
	}
	return symbol;
}
