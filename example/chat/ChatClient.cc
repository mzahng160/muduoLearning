#include "ChatCodec.h"
#include "TcpClient.h"
#include "thread/Mutex.h"
#include "EventLoopThread.h"

#include <stdio.h>
#include <boost/bind.hpp>

using namespace muduo;

class ChatClient
{
public:
	ChatClient(EventLoop* loop, const InetAddress& serverAddr)
		: client_(loop, serverAddr),
		  codec_(boost::bind(&ChatClient::onStringMessage, this, _1, _2, _3))
	{
		client_.setConnectionCallback(
			boost::bind(&ChatClient::onConnection, this, _1));
		client_.setMessageCallback(
			boost::bind(&LengthChatCodec::onMessage, &codec_, _1, _2, _3));
		client_.enableRetry();

	}
	//~ChatClient();
	
	void connect()
	{
		client_.connect();
	}

	void disconnect()
	{
		client_.disconnect();
	}

	void write(const std::string message)
	{
		MutexLockGuard lock(mutex_);
		if(connection_)
		{
			codec_.send(get_pointer(connection_), message);
		}
	}

private:
	void onConnection(const TcpConnectionPtr& conn)
	{
		MutexLockGuard lock(mutex_);
		if(conn->connected())
		{
			connection_ = conn;
		}
		else
		{
			connection_.reset();
		}
	}
	
	void onStringMessage(const TcpConnectionPtr& conn,
			const std::string& message, Timestamp)
	{
		printf(" <<< %s\n", message.c_str());
	}

	TcpClient client_;
	LengthChatCodec codec_;
	MutexLock mutex_;
	TcpConnectionPtr connection_;	
};

int main(int argc, char* argv[])
{
	LOG_INFO << "PID = " << getpid();

	if(argc > 2)
	{
		EventLoopThread loopThread;
		uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
		InetAddress serverAddr(argv[1], port);

		ChatClient client(loopThread.startLoop(), serverAddr);
		client.connect();

		std::string line;
		while(std::getline(std::cin, line))
		{
			client.write(line);
		}
		client.disconnect();
		//CurrentThread::sleepUsec(1000*1000);		
	}
	else
	{
		printf("Unsage: %s host_ip port\n", argv[0]);
	}
}