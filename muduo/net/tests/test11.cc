#include "/xmuduo/muduo/net/TcpServer.h"
#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/InetAddress.h"

#include <boost/bind.hpp>
#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

class Server
{
public:
	Server(EventLoop* loop,const InetAddress& listenAddr,
			const string& nameArg,int numThreads)
	:	server_(loop,listenAddr,nameArg)
	{
		server_.setConnectionCallback(boost::bind(&Server::onConnection,this,_1));
		server_.setMessageCallback(boost::bind(&Server::onMessage,this,_1,_2,_3));
		server_.setWriteCompleteCallback(boost::bind(&Server::writeCompleteCallback,this,_1));
		//server_.setThreadNum(numThreads);
		for(int i = 0; i < 10; i++)
			message_.append("this is a chargen attack\n");
	}
	
	void start()
	{
		server_.start();
	}
private:

	void onConnection(const TcpConnectionPtr& conn)
	{
		if(conn -> connected())
		{
			printf("onConnection() : new connection [%s] from %s\n",
				conn -> name().c_str(),
				conn -> peerAddress().toIpPort().c_str());
			conn->setTcpNoDelay(true);
			conn->send(message_);
		}
		else
		{
			printf("onConnection() : connection [%s] is down\n",
				conn -> name().c_str());
		}	
	}

	void onMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp receiveTime)
	{
		muduo::string msg(buf->retrieveAllAsString());	
		conn->send(msg);
	}

	void writeCompleteCallback(const TcpConnectionPtr& conn)
	{
		printf("----------writeCompleteCallback is called-------------------\n");
		conn->send(message_);
	}
	
	TcpServer server_;
	muduo::string message_;
};

int main(void)
{
	EventLoop loop;
	InetAddress addr(9527);
	Server server(&loop,addr,"server",4);
	server.start();
	loop.loop();
	return 0;
}
