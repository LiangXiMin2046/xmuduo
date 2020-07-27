#ifndef MUDUO_NET_TCPCONNECTION_H
#define MUDUO_NET_TCPCONNECTION_H

#include "/xmuduo/muduo/base/Mutex.h"
#include "/xmuduo/muduo/base/StringPiece.h"
#include "/xmuduo/muduo/base/Types.h"
#include "/xmuduo/muduo/net/Callbacks.h"
//#include "/xmuduo/muduo/net/Buffer.h"
#include "/xmuduo/muduo/net/InetAddress.h"

//#include <boost/any.hpp>
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
	
	void setConnectionCallback(const ConnectionCallback& cb)
	{ connectionCallback_ = cb; }

	void setMessageCallback(const MessageCallback& cb)
	{ messageCallback_ = cb; }

	//called when TcpServer accepted a new connection
	void connectEstablished();	
private:
	enum stateE {/*kDisConnected*/kConnecting,kConnected/*,kDisConnecting*/};
	void handleRead(Timestamp receiveTime);
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
};

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

}
}
#endif
