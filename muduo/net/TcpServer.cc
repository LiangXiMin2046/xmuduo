#include "/xmuduo/muduo/net/TcpServer.h"

#include "/xmuduo/muduo/base/Logging.h"
#include "/xmuduo/muduo/net/Acceptor.h"
#include "/xmuduo/muduo/net/EventLoop.h"
//#include "/xmuduo/net/EventLoopThreadPool.h"
#include "/xmuduo/muduo/net/SocketsOps.h"

#include <boost/bind.hpp>
#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

TcpServer::TcpServer(EventLoop* loop,
					 const InetAddress& listenAddr,
					 const string& nameArg)
	:	loop_(CHECK_NOTNULL(loop)),
		hostport_(listenAddr.toIpPort()),
		name_(nameArg),
		acceptor_(new Acceptor(loop,listenAddr)),
		/*threadPool_(new EventLoopThreadPool(loop)),
 		* connectionCallback_(defaultConnectionCallback),
 		* messageCallback_(defaultConnectionback),*/
		started_(false),
		nextConnId_(1)
{
	acceptor_ -> setNewConnectionCallback(
		boost::bind(&TcpServer::newConnection,this,_1,_2));
}

TcpServer::~TcpServer()
{
	loop_ -> assertInLoopThread();
	LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destructing";
}

void TcpServer::start()
{
	if(!started_)
	{
		started_ = true;
	}
	if(!acceptor_ -> listenning())
	{
		loop_ -> runInLoop(boost::bind(&Acceptor::listen,
				 get_pointer(acceptor_)));
	}
}

void TcpServer::newConnection(int sockfd,const InetAddress& peerAddr)
{
	loop_ -> assertInLoopThread();
	char buf[32];
	snprintf(buf,sizeof buf,":%s#%d",hostport_.c_str(),nextConnId_);
	++nextConnId_;
	string connName = name_ + buf;

	LOG_INFO << "TcpServerLLnewConnection [" << name_
			 << "] - new connection [" << connName
			 << "] from " << peerAddr.toIpPort();

	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	
	TcpConnectionPtr conn(new TcpConnection(loop_,
											connName,
											sockfd,
											localAddr,
											peerAddr));
	connections_[connName] = conn;
	conn -> setConnectionCallback(connectionCallback_);
	conn -> setMessageCallback(messageCallback_);

	conn -> connectEstablished();
}
