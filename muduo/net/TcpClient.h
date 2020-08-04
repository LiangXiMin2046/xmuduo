#ifndef MUDUO_NET_TCPCLIENT_H
#define MUDUO_NET_TCPCLIENT_H

#include <boost/noncopyable.hpp>

#include "/xmuduo/muduo/base/Mutex.h"
#include "/xmuduo/muduo/net/TcpConnection.h"

namespace muduo
{
namespace net
{

class Connector;
typedef boost::shared_ptr<Connector> ConnectorPtr;

class TcpClient
{
public:
	TcpClient(EventLoop* loop,
			  const InetAddress& serverAddr,
			  const string& name);
	~TcpClient();
	
	void connect();
	void disconnect();
	void stop();

	TcpConnectionPtr connection() const
	{
		MutexLockGuard lock(mutex_);
		return connection_;
	}

	bool retry() const;

	void enableRetry() {  retry_ = true;  }

	//set Connection callback
	//not thread safe
	void setConnectionCallback(const ConnectionCallback& cb)
	{  connectionCallback_ = cb;  }

	//Set message callback.
	//Not thread safe.
	void setMessageCallback(const MessageCallback& cb)
	{	messageCallback_ = cb;  }

	void setWriteCompleteWriteCallback(const WriteCompleteCallback& cb)
	{	writeCompleteCallback_ = cb;  }

private:
	//not thread safe
	void newConnection(int sockfd);
	void removeConnection(const TcpConnectionPtr& conn);

	EventLoop* loop_;
	ConnectorPtr connector_;
	const string name_; 
	ConnectionCallback connectionCallback_;  //on connection
	MessageCallback messageCallback_;	//on message
	WriteCompleteCallback writeCompleteCallback_; 
	bool retry_;
	bool connect_;
	int nextConnId_;
	mutable MutexLock mutex_;
	TcpConnectionPtr connection_;
};

}
}

#endif
