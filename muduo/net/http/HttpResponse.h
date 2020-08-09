#ifndef MUDUO_NET_HTTP_HTTPRESPONSE_H
#define MUDUO_NET_HTTP_HTTPRESPONSE_H

#include "/xmuduo/muduo/base/copyable.h"
#include "/xmuduo/muduo/base/Types.h"

#include <map>

namespace muduo
{
namespace net
{
class Buffer;
class HttpResponse : public muduo::copyable
{
public:
	enum HttpStatusCode
	{
		kUnknown,
		k200Ok = 200,//success
		k301MovedPernanently = 301,
		k400BadRequest = 400,  //error request
		k404NotFound = 404, //classical error
	};

	explicit HttpResponse(bool close)
		:	statusCode_(kUnknown),
			closeConnection_(close)
	{
	}

	void setStatusCode(HttpStatusCode code)
	{  statusCode_ = code;  }

	void setStatusMessage(const string& message)
	{  statusMessage_ = message;  }

	void setCloseConnection(bool on)
	{  closeConnection_ = on;  }

	bool closeConnection() const
	{  return closeConnection_;  }

	void setContentType(const string& contentType)
	{  addHeader("Content-Type",contentType);  }
	void addHeader(const string&key,const string& value)
	{  headers_[key] = value;  }

	void setBody(const string& body)
	{  body_ = body;  }

	void appendToBuffer(Buffer* output) const; //append response to buffer
private:
	std::map<string,string> headers_;
	HttpStatusCode statusCode_;
	string statusMessage_;
	bool closeConnection_;
	string body_;
};

}
}

#endif
