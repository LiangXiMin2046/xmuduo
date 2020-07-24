#ifndef MUDUO_NET_ACCEPTOR_H
#define MUDUO_NET_ACCEPTOR_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "/xmuduo/muduo/net/Channel.h"
#include "/xmuduo/muduo/net/Socket.h"

namespace muduo
{
namespace net
{
class EventLoop;
class InetAddress;

class Acceptor : boost::noncopyable
{
public:
	typedef boost::function<void(int sockfd,
							const InetAddress&)> NewConnectionCallback;
	Acceptor(EventLoop* loop,const InetAddress& listenAddr);
	~Acceptor();

	void setNewConnectionCallback(const NewConnectionCallback& cb)
	{ newConnectionCallback_ = cb; }

	bool listenning() const {return listenning_;}
	void listen();
private:
	void handleRead();
	
	EventLoop* loop_;
	Socket acceptSocket_;
	Channel acceptChannel_;
	NewConnectionCallback newConnectionCallback_;
	bool listenning_;
	int idleFd_;
};

}
}

#endif
