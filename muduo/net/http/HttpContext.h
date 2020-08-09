#ifndef MUDUO_NET_HTTP_HTTPCONTEXT_H
#define MUDUO_NET_HTTP_HTTPCONTEXT_H

#include "/xmuduo/muduo/base/copyable.h"
#include "/xmuduo/muduo/net/http/HttpRequest.h"

namespace muduo
{
namespace net
{
class Buffer;

class HttpContext : public muduo::copyable
{
public:
	enum HttpRequestParseState
	{
		kExpectRequestLine,
		kExpectHeaders,
		kExpectBody,
		kGotAll,
	};

	HttpContext()
		:	state_(kExpectRequestLine)
	{
	}
	
	bool expectRequestLine() const
	{  return state_ == kExpectRequestLine;  }

	bool expectHeaders() const
	{  return state_== kExpectHeaders;  }

	bool expectBody() const
	{  return state_ == kExpectBody;  }

	bool gotAll() const
	{  return state_ == kGotAll;  }

	void receiveRequestLine()
	{	state_ = kExpectHeaders;  }

	void receiveHeaders()
	{	state_ = kGotAll;  }

	void reset()
	{
		state_ = kExpectRequestLine;
		HttpRequest dummy;
		request_.swap(dummy);
	}

	const HttpRequest& request() const
	{  return request_;  }

	HttpRequest& request()
	{  return request_;  }

private:
	HttpRequestParseState state_; 
	HttpRequest request_;	//http request
};

}
}
#endif
