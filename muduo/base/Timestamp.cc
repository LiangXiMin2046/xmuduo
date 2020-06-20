#include "/xmuduo/muduo/base/Timestamp.h"

#include<sys/time.h>
#include<stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

#include <boost/static_assert.hpp>

using namespace muduo;

BOOST_STATIC_ASSERT(sizeof(Timestamp) == sizeof(int64_t));

Timestamp::Timestamp(int64_t microseconds) : microSecondsSinceEpoch_(microseconds){}

string Timestamp::toString() const
{
	char buf[32];
	int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
	int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
	snprintf(buf,sizeof(buf)-1,"%" PRId64 ".%06" PRId64 "",seconds,microseconds);
	return buf;
}

string Timestamp::toFormattedString() const
{
	char buf[32];
	time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
	int microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
	struct tm tm_time;
	gmtime_r(&seconds,&tm_time);
	snprintf(buf,sizeof(buf)-1,"%04d%02d%2d:%02d:%02d:%02d.%06d",tm_time.tm_year+1900,tm_time.tm_mon + 1,tm_time.tm_mday,tm_time.tm_hour,tm_time.tm_min,tm_time.tm_sec,microseconds);
	return buf;
}

Timestamp Timestamp::now()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	int64_t seconds = static_cast<int64_t>(tv.tv_sec);
	int64_t microseconds = static_cast<int64_t>(tv.tv_usec);
	return Timestamp(seconds * kMicroSecondsPerSecond + microseconds);
}

Timestamp Timestamp::invalid()
{
	return Timestamp();
}

