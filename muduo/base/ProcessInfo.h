#ifndef MUDUO_BASE_PROCESSINFO_H
#define MUDUO_BASE_PROCESSINFO_H

#include "/xmuduo/muduo/base/Types.h"
#include "/xmuduo/muduo/base/Timestamp.h"
#include<vector>

namespace muduo
{

namespace ProcessInfo
{
	pid_t pid();
	string pidString();
	uid_t uid();
	string username();
	uid_t euid();
	Timestamp startTime();
	
	string hostname();
	//read /proc/self/status
	string procStatus();
	int openedFiles();
	int maxOpenFiles();
	
	int numThreads();
	std::vector<pid_t> threads();
}

}
#endif
