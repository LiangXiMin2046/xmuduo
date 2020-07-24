#include "/xmuduo/muduo/net/Acceptor.h"

#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/InetAddress.h"
#include "/xmuduo/muduo/net/SocketsOps.h"

#include <boost/bind.hpp>

#include <errno.h>
#include <fcntl.h>

using namespace muduo;
using namespace muduo::net;

Acceptor::Acceptor(EventLoop* loop,const InetAddress& listenAddr)
	:	loop_(loop),
		acceptSocket_(sockets::createNonblockingOrDie()),
		acceptChannel_(loop,acceptSocket_.fd()),
		listenning_(false),
		idleFd_(::open("/dev/null",O_RDONLY | O_CLOEXEC))
{
	assert(idleFd_ >= 0);
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.bindAddress(listenAddr);
	acceptChannel_.setReadCallback(boost::bind(&Acceptor::handleRead,this));	
}

Acceptor::~Acceptor()
{
	acceptChannel_.disableAll();
	acceptChannel_.remove();
	::close(idleFd_);
}

void Acceptor::listen()
{
	loop_ -> assertInLoopThread();
	listenning_ = true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
	loop_ -> assertInLoopThread();
	InetAddress peerAddr(0);
	int connfd = acceptSocket_.accept(&peerAddr);
	if(connfd >= 0)
	{
		if(newConnectionCallback_)
		{
			newConnectionCallback_(connfd,peerAddr);
		}
		else
		{
			sockets::close(connfd);
		}	
	}
	else
	{
		if(errno = EMFILE)
		{
			::close(idleFd_);
			idleFd_ = ::accept(acceptSocket_.fd(),NULL,NULL);
			::close(idleFd_);
			idleFd_ = ::open("/dev/null",O_RDONLY | O_CLOEXEC);
		}
	}
}
