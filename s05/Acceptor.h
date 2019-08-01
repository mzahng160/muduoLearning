#ifndef MUDUO_NET_ACCEPTOR_H
#define MUDUO_NET_ACCEPTOR_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "Channel.h"
#include "Socket.h"

namespace muduo
{
	class EventLoop;
	class InetAddress;

	class Acceptor : boost::noncopyable
	{
	public:		

		typedef boost::function<void (int sockfd, 
					const InetAddress&)> NewConnectionCallback;

		Acceptor(EventLoop* loop, const InetAddress& listenAddress);

		void setNewConnectionCallback(const NewConnectionCallback& cb)
		{ newConnectionCallback_ = cb; }

		bool listenning() const { return listenning_; }
		void listen();

	private:
		void handleRead();

		EventLoop* loop_;
		Socket acceptSocket_;
		Channel acceptChannel_;
		NewConnectionCallback newConnectionCallback_;
		bool listenning_;
	};
}

#endif