#ifndef MUDUO_BASE_LOGFILE_H
#define MUDUO_BASE_LOGFILE_H

#include "/xmuduo/muduo/base/Mutex.h"
#include "/xmuduo/muduo/base/Types.h"

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace muduo
{
class LogFile : boost::noncopyable
{
public:
	LogFile(const string& basename,size_t rollSize,
				bool threadSafe = true,int flushInterval = 3);
	~LogFile();
	
	void append(const char* logline,int len);
	void flush();

private:
	void append_unlocked(const char* logline,int len);
	
	static string getLogFileName(const string& basename,time_t* now);
	void rollFile();

	const string basename_;//basename of logfile;
	const size_t rollSize_; //maxsize of a roll logfile
	const int flushInterval_; //

	int count_;

	boost::scoped_ptr<MutexLock> mutex_;
	time_t startOfPeriod_; 
	time_t lastRoll_;
	time_t lastFlush_;
	class File;
	boost::scoped_ptr<File> file_;

	const static int kCheckTimeRoll_ = 1024;
	const static int kRollPerSeconds_ = 60 * 60 * 24;
};
}

#endif
