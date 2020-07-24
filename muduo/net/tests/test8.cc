#include "/xmuduo/muduo/net/Acceptor.h"
#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/InetAddress.h"
#include "/xmuduo/muduo/net/SocketsOps.h"

#include <fcntl.h>
#include <stdio.h>
#include <time.h>

using namespace muduo;
using namespace muduo::net;

void newConnection(int sockfd,const InetAddress& addr)
{
	//printf("%d\n",sockfd);
	printf("ip and port of new connection : %s\n",addr.toIpPort().c_str());
	time_t nowTime = time(NULL);
	char buf[26];
	snprintf(buf,sizeof(buf),"%s",::ctime(&nowTime));
	sockets::write(sockfd,buf,sizeof(buf));
	//::write(sockfd,"hello",5);
	sockets::close(sockfd);		
}

int main(void)
{
	EventLoop loop;

	InetAddress addr1(9981);
	InetAddress addr2(9527);

	Acceptor acceptor1(&loop,addr1);
	Acceptor acceptor2(&loop,addr2);

	acceptor1.setNewConnectionCallback(newConnection);
	acceptor1.listen();

	acceptor2.setNewConnectionCallback(newConnection);
	acceptor2.listen();
	
	loop.loop();
	return 0;
}
