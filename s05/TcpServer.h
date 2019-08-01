#ifndef MUDUO_NET_TCPSERVER_H
#define MUDUO_NET_TCPSERVER_H

#include "Callbacks.h"
#include "TcpConnection.h"

#include <boost/noncopyable.hpp>

namespace muduo
{
	class Acceptor;
	class EventLoop;

	class TcpServer : boost::noncopyable
	{
	public:
		TcpServer(EventLoop* loop, const InetAddress& listenAddr);
		~TcpServer();
		
	};
}

#endif