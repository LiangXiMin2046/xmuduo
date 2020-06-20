#ifndef _MUDUO_BASE_TIMESTAMP_H
#define _MUDUO_BASE_TIMESTAMP_H

#include "/muduoday/day1/muduo/base/Types.h"
#include "/muduoday/day1/muduo/base/copyable.h"

#include<boost/operators.hpp>

namespace muduo
{
//Timestamp in UTC
//Timestamp is recommended to pass by value
class Timestamp : public muduo::copyable,
                  public boost::less_than_comparable<Timestamp>
{
public:
	//construct a unvalid Timestamp
	Timestamp():microSecondsSinceEpoch_(0){}
	//construct a specific time Timestamp
	explicit Timestamp(int64_t microSecondsSinceEpoch);
	void swap(Timestamp &that)
	{
		std::swap(microSecondsSinceEpoch_,that.microSecondsSinceEpoch_);
	}
	string toString() const;
	string toFormattedString() const;
	bool valid() const {return microSecondsSinceEpoch_ > 0;}
	int64_t microSecondsSinceEpoch()const {return microSecondsSinceEpoch_;}
	time_t secondsSinceEpoch() const {return static_cast<time_t> (microSecondsSinceEpoch_ / kMicroSecondsPerSecond);}
	
	static Timestamp now();
	static Timestamp invalid();

	static const int kMicroSecondsPerSecond = 1000 * 1000;
	
private:
	int64_t microSecondsSinceEpoch_;
};

inline bool operator < (Timestamp lhs,Timestamp rhs) {
	return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator == (Timestamp lhs,Timestamp rhs) {
	return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline double timeDifference(Timestamp high,Timestamp low){
	int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
	return static_cast<double> (diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTime(Timestamp timestamp,double seconds){
	int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
	return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}
}

#endif

