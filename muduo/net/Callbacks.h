#ifndef MUDUO_NET_CALLBACKS_H
#define MUDUO_NET_CALLBACKS_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "/xmuduo/muduo/base/Timestamp.h"

namespace muduo
{
namespace net
{
class TcpConnection;
class Buffer;

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef boost::function<void()> TimerCallback;

typedef boost::function<void(const TcpConnectionPtr&)> ConnectionCallback;

typedef boost::function<void(const TcpConnectionPtr&)> CloseCallback;

typedef boost::function<void(const TcpConnectionPtr&,
                             Buffer* buffer,
							 Timestamp receivedTime)> MessageCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);

void defaultMessageCallback(const TcpConnectionPtr& conn,
							Buffer* buf,
							Timestamp receiveTime);
}
}

#endif
