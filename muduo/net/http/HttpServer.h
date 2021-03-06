#ifndef MUDUO_NET_HTTPSERVER_H
#define MUDUO_NET_HTTPSERVER_H

#include "/xmuduo/muduo/net/TcpServer.h"
#include <boost/noncopyable.hpp>

namespace muduo
{
namespace net
{
class HttpRequest;
class HttpResponse;
///simple HTTP server for reporting status of program
///dot not support full Http1.1
class HttpServer : boost::noncopyable
{
public:
	typedef boost::function<void(const HttpRequest&,HttpResponse*)> HttpCallback;

	HttpServer(EventLoop* loop,const InetAddress& listenAddr,
			   const string& name);

	~HttpServer();

	void setHttpCallback(const HttpCallback& cb)
	{
		httpCallback_ = cb;
	}

	void setThreadNum(int numThreads)
	{
		server_.setThreadNum(numThreads);
	}
	
	void start();
private:
	void onConnection(const TcpConnectionPtr& conn);
	void onMessage(const TcpConnectionPtr& conn,
				   Buffer* buf,
				   Timestamp receiveTime);
	void onRequest(const TcpConnectionPtr&,const HttpRequest&);

	TcpServer server_;
	HttpCallback httpCallback_;
};

}
}

#endif
