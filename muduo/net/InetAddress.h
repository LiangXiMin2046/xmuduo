#ifndef MUDUO_NET_INETADDRESS_H
#define MUDUO_NET_INETADDRESS_H

#include "/xmuduo/muduo/base/copyable.h"
#include "/xmuduo/muduo/base/StringPiece.h"

#include <netinet/in.h>

namespace muduo
{
namespace net
{
class InetAddress : public muduo::copyable
{
public:

	//mostly uesd in TcpServer listening
	explicit InetAddress(uint16_t port);
	
	InetAddress(const StringPiece& ip,uint16_t port);

	InetAddress(const struct sockaddr_in& addr)
		:	addr_(addr)
	{ }
	
	string toIp() const;
	string toIpPort() const;

	string toHostPort() const __attribute__((deprecated))
	{ return toIpPort(); }

	const struct sockaddr_in& getSockAddrInet() const {return addr_;}

	void setSockAddrInet(const struct sockaddr_in& addr) {addr_ = addr;}

	uint32_t ipNetEndian() const {return addr_.sin_addr.s_addr;}
	uint16_t portNetEndian() const {return addr_.sin_port;}

private:
	struct sockaddr_in addr_;
};

}
}

#endif
