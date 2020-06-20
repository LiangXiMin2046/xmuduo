#include "/xmuduo/muduo/base/Timestamp.h"
#include<stdio.h>
#include<vector>

using muduo::Timestamp;

void passByConstReference(const Timestamp& x)
{
  printf("%s\n", x.toString().c_str());
}

void passByValue(Timestamp x)
{
  printf("%s\n", x.toString().c_str());
}

void benchmark()
{
	int count = 1000 * 1000;
	std::vector<Timestamp> vec;
	vec.reserve(count);
	for(int i = 0; i < count; ++i)
		vec.push_back(Timestamp::now());
	printf("vec.front:%s\n",vec.front().toString().c_str());
	printf("vec.back:%s\n",vec.back().toString().c_str());
	printf("%f\n", timeDifference(vec.back(), vec.front()));	
}

int main(void)
{
	Timestamp now(Timestamp::now());
	passByConstReference(now);
	passByValue(now);
	benchmark();
	return 0;
}

