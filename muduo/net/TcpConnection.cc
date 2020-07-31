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


void muduo::net::defaultConnectionCallback(const TcpConnectionPtr& conn)
{
	LOG_TRACE << conn -> localAddress().toIpPort() << " -> "
			  << conn -> peerAddress().toIpPort() << " is "
			  << (conn -> connected() ? "UP" : "DOWN");
}


void muduo::net::defaultMessageCallback(const TcpConnectionPtr&,
										Buffer* buf,
										Timestamp)
{
	buf -> retrieveAll();
}
 
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

//thread safe
void TcpConnection::send(const void* data,size_t len)
{
	if(state_ == kConnected)
	{
		if(loop_ -> isInLoopThread())
		{
			sendInLoop(data,len);
		}
		else
		{
			string message(static_cast<const char*>(data),len);
			loop_ -> runInLoop(boost::bind(&TcpConnection::sendInLoop,
							  this,message));
		}
	}
}

void TcpConnection::send(const StringPiece& message)
{
	if(state_ == kConnected)
	{
		if(loop_ -> isInLoopThread())
		{
			sendInLoop(message);
		}
		else
		{
			loop_ -> runInLoop(boost::bind(&TcpConnection::sendInLoop,
							  this,message.as_string()));
		}
	}
}

void TcpConnection::send(Buffer* buf)
{
	if(state_ == kConnected)
	{
		if(loop_ -> isInLoopThread())
		{
			sendInLoop(buf->peek(),buf->readableBytes());
			buf -> retrieveAll();
		}
		else
		{
			loop_ -> runInLoop(boost::bind(&TcpConnection::sendInLoop,
							  this,buf->retrieveAllAsString()));
		}
	}
}

void TcpConnection::sendInLoop(const StringPiece& message)
{
	sendInLoop(message.data(),message.size());
}

void TcpConnection::sendInLoop(const void* data,size_t len)
{
	loop_ -> assertInLoopThread();
	sockets::write(channel_->fd(),data,len);
	/*
 *
 *
 * */
}

void TcpConnection::shutdown()
{
	if(state_ == kConnected)
	{
		setState(kDisConnecting);
		loop_->runInLoop(boost::bind(&TcpConnection::shutdownInLoop,this));
	}
}

void TcpConnection::shutdownInLoop()
{
	loop_->assertInLoopThread();
	if(!channel_->isWriting())
	{
		socket_->shutdownWrite();
	}
}

void TcpConnection::setTcpNoDelay(bool on)
{
	socket_ -> setTcpNoDelay(on);
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
	ssize_t n = inputBuffer_.readFd(channel_->fd(),&savedErrno);
	if(n > 0)
	{
		messageCallback_(shared_from_this(),&inputBuffer_,receiveTime);
	}
	else if(n == 0)
	{
		LOG_TRACE << "hanle closing...";
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
