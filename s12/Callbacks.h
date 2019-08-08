#ifndef MUDUO_NET_CALLBACKS_H
#define MUDUO_NET_CALLBACKS_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "datetime/Timestamp.h"
namespace muduo
{
	class Buffer;
	class TcpConnection;
	class Connector;

	typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
	typedef boost::shared_ptr<Connector> ConnectorPtr;
	

	typedef boost::function<void()> TimerCallback;
	typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
	typedef boost::function<void (const TcpConnectionPtr&,
									Buffer* buf, 
									Timestamp)> MessageCallback;
	typedef boost::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
	typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;	
}

#endif //MUDUO_NET_CALLBACKS_H