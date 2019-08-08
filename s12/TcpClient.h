#ifndef MUDUO_NET_TCPCLIENT_H
#define MUDUO_NET_TCPCLIENT_H

#include <boost/noncopyable.hpp>

namespace muduo
{
	class Connector;
	class TcpClient : boost::noncopyable
	{
	public:
		TcpClient(EventLoop* loop, const InetAddress& listenAddr);
		~TcpClient();

		void setConnectionCallback(const ConnectionCallback& cb)
		{ connectionCallback_ = cb; }

		void setMessageCallback(const MessageCallback& cb)
		{ messageCallback_ = cb; }

		void enableRetry();
		void connect();

	private:
		EventLoop* loop_;
		
	};
}

#endif