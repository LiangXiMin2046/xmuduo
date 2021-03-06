#include "/xmuduo/muduo/net/Connector.h"

#include "/xmuduo/muduo/base/Logging.h"
#include "/xmuduo/muduo/net/Channel.h"
#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/SocketsOps.h"

#include <boost/bind.hpp>

#include <errno.h>

using namespace muduo;
using namespace muduo::net;

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop,const InetAddress& serverAddr)
	:	loop_(loop),
		serverAddr_(serverAddr),
		connect_(false),
		state_(kDisconnected),
		retryDelayMs_(kInitRetryDelayMs)
{
	LOG_DEBUG << "ctor [" << this << "]";
}

Connector::~Connector()
{
	LOG_DEBUG << "dtor[" << this << "]";
	assert(!channel_);
}

void Connector::start()
{
	connect_ = true;
	loop_->runInLoop(boost::bind(&Connector::startInLoop,this));
}

void Connector::startInLoop()
{
	loop_->assertInLoopThread();
	assert(state_ == kDisconnected);
	if(connect_)
	{
		connect();
	}
	else
	{
		LOG_DEBUG << "do not connect";
	}
}

void Connector::stop()
{
	connect_ = false;
	loop_->runInLoop(boost::bind(&Connector::stopInLoop,this));
}

void Connector::stopInLoop()
{
	loop_->assertInLoopThread();
	if(state_ == kConnecting)
	{
		setState(kDisconnected);
		int sockfd = removeAndResetChannel();
		retry(sockfd);// call close()
	}
}

void Connector::connect()
{
	int sockfd = sockets::createNonblockingOrDie();
	int ret = sockets::connect(sockfd,serverAddr_.getSockAddrInet());
	int savedErrno = (ret == 0) ? 0 : errno;
	switch (savedErrno)
	{
		case 0:
		case EINPROGRESS:  //connect unsuccessfully
		case EINTR:
		case EISCONN:
			connecting(sockfd);
			break;

		case EAGAIN:
		case EADDRINUSE:
		case EADDRNOTAVAIL:
		case ECONNREFUSED:
		case ENETUNREACH:
			retry(sockfd);
			break;

		case EACCES:
		case EPERM:
		case EAFNOSUPPORT:
		case EALREADY:
		case ENOTSOCK:
			LOG_SYSERR << 
			"connect error in Connector::startInLoop " << savedErrno;
			sockets::close(sockfd);
			break;
		
		default:
			LOG_SYSERR << 
			"unexpected error in Connctor::startInLoop" << savedErrno;
			sockets::close(sockfd);
			//connectErrorCallback_();
		break;
	}

}

void Connector::restart()
{
	loop_->assertInLoopThread();
	setState(kDisconnected);
	retryDelayMs_ = kInitRetryDelayMs;
	connect_ = true;
	startInLoop();
}

void Connector::connecting(int sockfd)
{
	LOG_TRACE << "connecting...";
	setState(kConnecting);
	assert(!channel_);
	//sockfd connect to channel
	channel_.reset(new Channel(loop_,sockfd));
	//set write callback func
	channel_->setWriteCallback(
		boost::bind(&Connector::handleWrite,this));
	channel_->setErrorCallback(
		boost::bind(&Connector::handleError,this));
	channel_->enableWriting();
}

int Connector::removeAndResetChannel()
{
	channel_->disableAll();
	channel_->remove();  //remove channel from poller
	int sockfd = channel_->fd();

	loop_->queueInLoop(boost::bind(&Connector::resetChannel,this));
	return sockfd;	
}

void Connector::resetChannel()
{
	channel_.reset();
}

void Connector::handleWrite()
{
	LOG_TRACE << "Connector::handleWrite " << state_;
	
	if(state_ == kConnecting)
	{
		int sockfd = removeAndResetChannel();
		//check
		int err = sockets::getSocketError(sockfd);
		if(err)
		{
			LOG_WARN << "Connector::handleWrite -SO_ERROR = "
					 << err << " " << strerror_tl(err);
			retry(sockfd);  //connect again
		}
		else if(sockets::isSelfConnect(sockfd))
		{
			LOG_WARN << "Connector::handleWrite -Self connect";
			retry(sockfd);
		}
		else
		{
			setState(kConnected);
			if(connect_)
			{
				newConnectionCallback_(sockfd);
			}
			else
			{
				sockets::close(sockfd);
			}
		}
	}
	else
	{
		assert(state_ == kDisconnected);
	}
}

void Connector::handleError()
{
	LOG_ERROR << "Connector::handleError";
	assert(state_ == kConnecting);
	int sockfd = removeAndResetChannel();
	int err = sockets::getSocketError(sockfd);
	LOG_TRACE << "SO_ERROR = " << err << " " << strerror_tl(err);
	retry(sockfd);
}

void Connector::retry(int sockfd)
{
	sockets::close(sockfd);
	setState(kDisconnected);
	if(connect_)
	{
		LOG_INFO << "Connector::retry - retry connecting to"
				 << serverAddr_.toIpPort() 
				 << " in " << retryDelayMs_ << " milliseconds. ";
		loop_->runAfter(retryDelayMs_/1000.0,
						boost::bind(&Connector::startInLoop,shared_from_this()));
		retryDelayMs_ = std::min(retryDelayMs_ * 2,kMaxRetryDelayMs);
	}
	else
	{
		LOG_DEBUG << "do not connect";
	}
}
