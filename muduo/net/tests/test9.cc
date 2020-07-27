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
			const string& nameArg)
	:	server_(loop,listenAddr,nameArg)
	{
		server_.setConnectionCallback(boost::bind(&Server::onConnection,this,_1));
		server_.setMessageCallback(boost::bind(&Server::onMessage,this,_1,_2,_3));
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
		}
		else
		{
			printf("onConnection() : connection [%s] is down\n",
				conn -> name().c_str());
		}	
}

	void onMessage(const TcpConnectionPtr& conn,const char* data,ssize_t len)
	{
		printf("onMessage() : received %zd bytes from connection [%s]\n",
			len,conn -> name().c_str());	
	}

	TcpServer server_;
};

int main(void)
{
	EventLoop loop;
	InetAddress addr(9527);
	Server server(&loop,addr,"server");	
	server.start();
	loop.loop();
	return 0;
}
