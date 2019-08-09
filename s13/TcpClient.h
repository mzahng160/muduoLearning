#ifndef MUDUO_NET_TCPCLIENT_H
#define MUDUO_NET_TCPCLIENT_H

#include <boost/noncopyable.hpp>

#include "TcpConnection.h"
#include "thread/Mutex.h"

namespace muduo
{
	class Connector;

	class TcpClient : boost::noncopyable
	{
	public:
		TcpClient(EventLoop* loop, const InetAddress& serverAddr);
		~TcpClient();

		void connect();
		void disconnect();
		void stop();

		TcpConnectionPtr connection() const
		{
			MutexLockGuard lock(mutex_);
			return connection_;
		}

		void retry() const;
		void enableRetry() { retry_ = true; }
		

		void setConnectionCallback(const ConnectionCallback& cb)
		{ connectionCallback_ = cb; }

		void setMessageCallback(const MessageCallback& cb)
		{ messageCallback_ = cb; }

		void setWriteCompleteCallback(const WriteCompleteCallback& cb)
		{ writeCompleteCallback_ = cb; }

	private:
		void newConnection(int sockfd);
		void removeConnection(const TcpConnectionPtr& conn);

		EventLoop* loop_;

		ConnectorPtr connector_;

		bool retry_;
		bool connect_;

		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;
		WriteCompleteCallback writeCompleteCallback_;

		int nextConnId_;
		mutable MutexLock mutex_;
		TcpConnectionPtr connection_;
		
	};
}

#endif