#ifndef MUDUO_NET_TCPCONNECTION_H
#define MUDUO_NET_TCPCONNECTION_H

#include "Callbacks.h"
#include "InetAddress.h"

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/scoped_ptr.hpp>

namespace muduo
{
	class Channel;
	class EventLoop;
	class Socket;
	
	class TcpConnection : boost::noncopyable,
							public boost::enable_shared_from_this<TcpConnection>
	{
	public:
		TcpConnection(EventLoop* loop, 
						const std::string& name,
						int sockfd, 
						const InetAddress& localAddress,
						const InetAddress& peerAddr);
		~TcpConnection();

		EventLoop* getLoop() const { return loop_; }
		const std::string& name() const { return name_; }
		const InetAddress& localAddress() { return localAddr_; }
		const InetAddress& peerAddress() { return peerAddr_; }
		bool connected() const {return state_ == kConnected; }

		void setConnectionCallback(const ConnectionCallback& cb)
		{ connectionCallback_ = cb; }

		void setMessageCallback(const MessageCallback& cb)
		{ messageCallback_ = cb; }

		void setCloseCallback(const CloseCallback& cb)
		{ closeCallback_ = cb; }

		void connectEstablished();

		void connectDestroyed();

	private:
		enum StateE { kConnecting, kConnected, kDisconnected,};

		void setState(StateE s) { state_ =  s; }
		void handleRead();
		void handleWrite();
		void handleClose();
		void handleError();

		EventLoop* loop_;
		std::string name_;
		StateE state_;

		boost::scoped_ptr<Socket> socket_;
		boost::scoped_ptr<Channel> channel_;

		InetAddress localAddr_;
		InetAddress peerAddr_;

		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;
		CloseCallback closeCallback_;
	};
}

#endif