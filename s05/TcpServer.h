#ifndef MUDUO_NET_TCPSERVER_H
#define MUDUO_NET_TCPSERVER_H

#include "Callbacks.h"
#include "TcpConnection.h"

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <map>

namespace muduo
{
	class Acceptor;
	class EventLoop;

	class TcpServer : boost::noncopyable
	{
	public:
		TcpServer(EventLoop* loop, const InetAddress& listenAddr);
		~TcpServer();

		void start();

		void setConnectionCallback(const ConnectionCallback& cb)
		{ connectionCallback_ = cb; }

		void setMessageCallback(const MessageCallback& cb)
		{ messageCallback_ = cb; }
		

	private:
		void newConnection(int sockfd, const InetAddress& peerAddr);

		EventLoop* loop_;
		const std::string name_;
		boost::scoped_ptr<Acceptor> acceptor_;
		bool start_;
		int nextConnId_;

		typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;
		ConnectionMap connections_;

		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;
	};
}

#endif