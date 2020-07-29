#include "/xmuduo/muduo/net/TcpServer.h"

#include "/xmuduo/muduo/base/Logging.h"
#include "/xmuduo/muduo/net/Acceptor.h"
#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/EventLoopThreadPool.h"
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
		threadPool_(new EventLoopThreadPool(loop)),
 		/* connectionCallback_(defaultConnectionCallback),
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
	for(ConnectionMap::iterator it(connections_.begin());
			it != connections_.end(); ++it)
	{
		TcpConnectionPtr conn = it->second;
		it->second.reset();
		conn -> getLoop() -> runInLoop(
				boost::bind(&TcpConnection::connectDestroyed,conn));
		conn.reset();
	}	
}

void TcpServer::setThreadNum(int numThreads)
{
	assert(0 <=  numThreads);
	threadPool_ -> setThreadNum(numThreads);
}

void TcpServer::start()
{
	if(!started_)
	{
		started_ = true;
		threadPool_ -> start(threadInitCallback_);
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
	EventLoop* ioLoop = threadPool_ -> getNextLoop();
	char buf[32];
	snprintf(buf,sizeof buf,":%s#%d",hostport_.c_str(),nextConnId_);
	++nextConnId_;
	string connName = name_ + buf;

	LOG_INFO << "TcpServerLLnewConnection [" << name_
			 << "] - new connection [" << connName
			 << "] from " << peerAddr.toIpPort();

	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	
	TcpConnectionPtr conn(new TcpConnection(ioLoop,
											connName,
											sockfd,
											localAddr,
											peerAddr));
	LOG_TRACE << " [1] usecount " << conn.use_count();
	connections_[connName] = conn;
	LOG_TRACE << " [2] usecount " << conn.use_count();
	conn -> setConnectionCallback(connectionCallback_);
	conn -> setMessageCallback(messageCallback_);

	conn -> setCloseCallback(
		boost::bind(&TcpServer::removeConnection,this,_1));

	ioLoop -> runInLoop(boost::bind(&TcpConnection::connectEstablished,conn));

	LOG_TRACE << " [5] usecount " << conn.use_count();
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
	loop_ -> runInLoop(boost::bind(&TcpServer::removeConnectionInLoop,this,conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
	loop_ -> assertInLoopThread();
	
	LOG_INFO << "TcpServer::removeConnctionInLoop [" << name_
		<< "] - connection " << conn -> name();

	LOG_TRACE << " [8] usecount = " << conn.use_count();
	size_t n = connections_.erase(conn->name());
	LOG_TRACE << " [9] usecount = " << conn.use_count();
	
	(void)n;
	assert(n == 1);
	EventLoop* ioLoop = conn->getLoop();
	ioLoop -> queueInLoop(boost::bind(&TcpConnection::connectDestroyed,conn));
	//loop_ -> queueInLoop(
		//boost::bind(&TcpConnection::connectDestroyed,conn));
	LOG_TRACE << " [10] usecount = " << conn.use_count();
}
