#include "ChatCodec.h"
#include "TcpServer.h"
#include "EventLoop.h"

#include <stdio.h>
#include <boost/bind.hpp>
#include <set>

using namespace muduo;

class ChatServer : boost::noncopyable
{
public:
	ChatServer(EventLoop* loop, const InetAddress& listenAddr)
			: server_(loop, listenAddr),
			  codec_(boost::bind(&ChatServer::onStringMessage, 
			  	this, _1, _2, _3))
	{
		server_.setConnectionCallback(
			boost::bind(&ChatServer::onConnection, this, _1));
		server_.setMessageCallback(
			boost::bind(&LengthChatCodec::onMessage, &codec_, _1, _2, _3));
	}

	//~ChatServer();

	void start()
	{
		server_.start();
	}
	
private:
	void onConnection(const TcpConnectionPtr& conn)
	{
		LOG_TRACE << conn->peerAddress().toHostPort() << " -> "
				<< conn->localAddress().toHostPort() << " is "
				<< (conn->connected() ? "up" : "down");
		//LOG_INFO << conn->getTcpInfo();
		conn->send("hello\n");
	}

	void onStringMessage(const TcpConnectionPtr&,
						const std::string& message, Timestamp)
	{
		for(ConnectionList::iterator it = connections_.begin();
			 it != connections_.end(); ++it)
		 {
		 	codec_.send(get_pointer(*it), message);
		 }
	}

	typedef std::set<TcpConnectionPtr> ConnectionList;
	TcpServer server_;
	LengthChatCodec codec_;
	ConnectionList connections_;
};

int main(int argc, char* argv[])
{
	LOG_INFO << "PID = " << getpid();

	if(argc > 1)
	{
		EventLoop loop;
		uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
		InetAddress serverAddr(port);
		ChatServer server(&loop, serverAddr);

		server.start();
		loop.loop();
	}
	else
	{
		printf("Usage %s port\n", argv[0]);
	}
	

}