#ifndef MUDUO_NET_CALLBACKS_H
#define MUDUO_NET_CALLBACKS_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "datetime/Timestamp.h"
namespace muduo
{
	class TcpConnection;

	typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

	typedef boost::function<void()> TimerCallback;
	typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
	typedef boost::function<void (const TcpConnectionPtr&,
									const char* data, 
									ssize_t len)> MessageCallback;
}

#endif //MUDUO_NET_CALLBACKS_H