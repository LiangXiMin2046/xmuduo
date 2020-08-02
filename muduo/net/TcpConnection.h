#ifndef MUDUO_NET_TCPCONNECTION_H
#define MUDUO_NET_TCPCONNECTION_H

#include "/xmuduo/muduo/base/Mutex.h"
#include "/xmuduo/muduo/base/StringPiece.h"
#include "/xmuduo/muduo/base/Types.h"
#include "/xmuduo/muduo/net/Callbacks.h"
#include "/xmuduo/muduo/net/Buffer.h"
#include "/xmuduo/muduo/net/InetAddress.h"

#include <boost/any.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace muduo
{
namespace net
{
class Channel;
class EventLoop;
class Socket;

//for client and server
class TcpConnection : boost::noncopyable,
					  public boost::enable_shared_from_this<TcpConnection>
{
public:
	TcpConnection(EventLoop* loop,
				  const string& name,
				  int sockfd,
				  const InetAddress& localAddr,
				  const InetAddress& peerAddr);

	~TcpConnection();
	
	EventLoop* getLoop() const {return loop_;}
	const string& name() const {return name_;}
	const InetAddress& localAddress() {return localAddr_;}
	const InetAddress& peerAddress() {return peerAddr_;}
	bool connected() const {return state_ == kConnected;}

	void send(const void* message,size_t len);
	void send(const StringPiece& message);
	void send(Buffer* message);
	void shutdown();
	void setTcpNoDelay(bool on);

	void setContext(const boost::any& context)
	{  context_ = context;  }

	const boost::any& getContext() const
	{  return context_;  }

	boost::any* getMutableContext()
	{  return &context_;  }
	
	void setConnectionCallback(const ConnectionCallback& cb)
	{  connectionCallback_ = cb;  }

	void setMessageCallback(const MessageCallback& cb)
	{  messageCallback_ = cb;  }
	
	void setHighWaterMarkCallback(const HighWaterMarkCallback& cb,size_t highWaterMark)
	{  highWaterMarkCallback_ = cb;  highWaterMark_ = highWaterMark;  }

	void setWriteCompleteCallback(const WriteCompleteCallback& cb)
	
	{  writeCompleteCallback_ = cb;  }

	Buffer* inputBuffer()
	{  return &inputBuffer_;  }

	//Internal use only
	void setCloseCallback(const CloseCallback& cb)
	{  closeCallback_ = cb;  }
	//called when TcpServer accepted a new connection
	void connectEstablished();	
	//called when TcpServer has removed TcpConnection from map
	void connectDestroyed();
private:
	enum stateE {kDisconnected,kConnecting,kConnected,kDisconnecting};
	void handleRead(Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();
	void sendInLoop(const StringPiece& message);
	void sendInLoop(const void* message,size_t len);
	void shutdownInLoop();
	void setState(stateE s) { state_ = s; }	
	
	EventLoop* loop_;
	string name_; //connection name
	stateE state_;
	boost::scoped_ptr<Socket> socket_;
	boost::scoped_ptr<Channel> channel_;
	InetAddress localAddr_;
	InetAddress peerAddr_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;

	HighWaterMarkCallback highWaterMarkCallback_;
	CloseCallback closeCallback_;
	size_t highWaterMark_;
	Buffer inputBuffer_;
	Buffer outputBuffer_;
	boost::any context_;
};

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

}
}
#endif
