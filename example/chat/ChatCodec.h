#ifndef MUDUO_EXAMPLE_CHAT_CODEC_H
#define MUDUO_EXAMPLE_CHAT_CODEC_H

#include "logging/Logging.h"
#include "TcpConnection.h"

#include <boost/noncopyable.hpp>
#include <functional>

class LengthChatCodec : boost::noncopyable
{
public:
	typedef std::function<void (const muduo::TcpConnectionPtr&,
							const std::string& message,
							muduo::Timestamp)> StringMessageCallback;

	explicit LengthChatCodec(const StringMessageCallback cb)
		: messageCallback_(cb)
	{
	}
	//~LengthChatCodec();
	void onMessage(const muduo::TcpConnectionPtr& conn,
					muduo::Buffer* buf,
					muduo::Timestamp receiveTime)
	{
		while(buf->readableBytes() > kHeaderLen)
		{
			const void* data = buf->peek();
			int32_t be32 = *static_cast<const int32_t*>(data);
			const int32_t len = muduo::sockets::networkToHost32(be32);
			if(len > 65536 || len < 0)
			{
				LOG_ERROR << "Invalid length" << len;
				conn->shutdown();
				break;
			}
			else if(buf->readableBytes() > len + kHeaderLen)
			{
				buf->retrieve(kHeaderLen);
				std::string message(buf->peek(), len);
				messageCallback_(conn, message, receiveTime);
				buf->retrieve(len);
			}
			else
			{
				break;
			}
		}
	}

	void send(muduo::TcpConnection* conn, std::string& message)
	{
		muduo::Buffer buf;
		buf.append(message.data(), message.size());
		int32_t len = static_cast<int32_t>(message.size());
		int32_t be32 = muduo::sockets::networkToHost32(len);
		buf.prepend(&be32, sizeof be32);
		conn->send(&buf);
	}
	

private:
	StringMessageCallback messageCallback_;
	const static size_t kHeaderLen = sizeof(int32_t);
	
};

#endif