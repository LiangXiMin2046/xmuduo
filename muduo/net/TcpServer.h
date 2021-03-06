#ifndef MUDUO_NET_TCPSERVER_H
#define MUDUO_NET_TCPSERVER_H

#include "/xmuduo/muduo/net/TcpConnection.h"
#include "/xmuduo/muduo/base/Types.h"

#include <map>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace muduo
{
namespace net
{
class Acceptor;
class EventLoop;
class EventLoopThreadPool;

//supports single-thread and thread pool model
class TcpServer : boost::noncopyable
{
public:
	typedef boost::function<void(EventLoop*)> ThreadInitCallback;
	//TcpServer(EventLoop* loop,const InetAddress& listenAddr);
	TcpServer(EventLoop* loop,const InetAddress& lostenAddr,
			  const string& nameArg);
	
	~TcpServer();

	const string& hostport() const {return hostport_;}
	const string& name() const {return name_;}
 
	//set number of threads
	void setThreadNum(int numThreads);
	void setThreadInitCallback(const ThreadInitCallback& cb)
	{  threadInitCallback_ = cb;  }
	//start the server
	void start();

	void setConnectionCallback(const ConnectionCallback& cb)
	{	connectionCallback_ = cb;	}

	void setMessageCallback(const MessageCallback& cb)
	{	messageCallback_ = cb;	}
	
	void setWriteCompleteCallback(const WriteCompleteCallback& cb)
	{  
		writeCompleteCallback_ = cb;
	}
private:
	void newConnection(int sockfd,const InetAddress& peerAddr);

	void removeConnection(const TcpConnectionPtr& conn);
	
	void removeConnectionInLoop(const TcpConnectionPtr& conn);
	typedef std::map<string,TcpConnectionPtr> ConnectionMap;
	EventLoop* loop_;
	const string hostport_;
	const string name_;
	boost::scoped_ptr<Acceptor> acceptor_;
	boost::scoped_ptr<EventLoopThreadPool> threadPool_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	ThreadInitCallback threadInitCallback_;
	bool started_;
	// in loop thread
	int nextConnId_;
	ConnectionMap connections_;
};

}
}

#endif
