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
	channel_ -> setReadCallback(
		boost::bind(&TcpConnection::handleRead,this,_1));
	
	channel_ -> setCloseCallback(
		boost::bind(&TcpConnection::handleClose,this));

	channel_ -> setErrorCallback(
		boost::bind(&TcpConnection::handleError,this));

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
	LOG_TRACE << "[3] usecount = " << shared_from_this().use_count();
	channel_ -> tie(shared_from_this());
	channel_ -> enableReading();
	
	connectionCallback_(shared_from_this()); 
	
	LOG_TRACE << "[4] usecount = " << shared_from_this().use_count();
}

void TcpConnection::connectDestroyed()
{
	loop_ -> assertInLoopThread();
	if(state_ == kConnected)
	{
		setState(kDisconnected);
		channel_->disableAll();
		
		connectionCallback_(shared_from_this());
	}

	channel_ -> remove();
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
	/**/
	loop_ -> assertInLoopThread();
	int savedErrno = 0;
	char buf[65535];
	ssize_t n = ::read(channel_ -> fd(),buf,sizeof(buf));
	if(n > 0)
	{
		messageCallback_(shared_from_this(),buf,n);
	}
	else if(n == 0)
	{
		handleClose();
	}
	else
	{
		errno = savedErrno;
		LOG_SYSERR << "TcpConnection::handleRead";
		handleError();
	}
}

void TcpConnection::handleClose()
{
	loop_ -> assertInLoopThread();
	LOG_TRACE << "fd = " << channel_ -> fd() << "state = " << state_;
	assert(state_ == kConnected || state_ == kDisConnecting);
	//don't close fd
	setState(kDisconnected);
	channel_ -> disableAll();

	TcpConnectionPtr guardThis(shared_from_this());
	connectionCallback_(guardThis);
	LOG_TRACE << "[7] usecount= " << guardThis.use_count();
	closeCallback_(guardThis);	//close callback set by TcpServer
	LOG_TRACE << "[11] usecount= " << guardThis.use_count();
}

void TcpConnection::handleError()
{
	int err = sockets::getSocketError(channel_ -> fd());
	LOG_ERROR << "TcpConnection::handleError [" << name_
		<< "] - SO_ERROR = " << err << " " << strerror_tl(err);
}
