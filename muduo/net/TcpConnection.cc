#include "/xmuduo/muduo/net/TcpConnection.h"

#include "/xmuduo/muduo/base/Logging.h"
#include "/xmuduo/muduo/net/Channel.h"
#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/Socket.h"
#include "/xmuduo/muduo/net/SocketsOps.h"

#include <boost/bind.hpp>

#include <errno.h>
#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

/*
void muduo::net::defaultConnectionCallBack(const TcpConnectionPtr& conn);
* /

/*
void muduo::net::defaultMessageCallBack(const TcpConnectionPtr&
										Buffer* buf,
										Timestamp);
*/
 
TcpConnection::TcpConnection(EventLoop* loop,
							 const string& nameArg,
							 int sockfd,
							 const InetAddress& localAddr,
							 const InetAddress& peerAddr)
	:	loop_(CHECK_NOTNULL(loop)),
		name_(nameArg),
		state_(kConnecting),
		socket_(new Socket(sockfd)),
		channel_(new Channel(loop,sockfd)),
		localAddr_(localAddr),
		peerAddr_(peerAddr)/*,
		highWaterMark_(64*1024*1024)*/
{
	channel_->setReadCallback(
		boost::bind(&TcpConnection::handleRead,this,_1));
	LOG_DEBUG << "TcpConnection::ctor[" << name_ << "] at" << this 
			<< " fd = "	<< sockfd;
	socket_ -> setKeepAlive(true);		
}

TcpConnection::~TcpConnection()
{
	LOG_DEBUG << "TcpConnection::dtor[" << name_ << "] at" << this 
				<< " fd=" <<  channel_ -> fd();
}

void TcpConnection::connectEstablished()
{
	loop_ -> assertInLoopThread();
	assert(state_ == kConnecting);
	setState(kConnected);
	channel_ -> tie(shared_from_this());
	channel_ -> enableReading();
	
	connectionCallback_(shared_from_this()); 
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
	/**/
	loop_ -> assertInLoopThread();
	char buf[65535];
	ssize_t n = ::read(channel_ -> fd(),buf,sizeof(buf));
	messageCallback_(shared_from_this(),buf,n);
}
