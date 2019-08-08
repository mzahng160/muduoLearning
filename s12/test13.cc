#include "InetAddress.h"
#include "EventLoop.h"
#include "TcpClient.h"

#include <stdio.h>

void onConnection(const muduo::TcpConnectionPtr& conn)
{
	if(conn->connected())
	{
		printf("onConnection(): tid=%d new connection[%s] from %s\n",
			muduo::CurrentThread::tid(),
			conn->name().c_str(),
			conn->peerAddress().toHostPort().c_str());

		if(!message.empty())
			conn->send(message);

		conn->shutdown();
	}
	else
	{
		printf("onConnection: tid=%d connection[%s] is down\n",
				muduo::CurrentThread::tid(),
				conn->name().c_str());
	}
}

void onMessage(const muduo::TcpConnectionPtr& conn,
				muduo::Buffer* buf,
				muduo::Timestamp receiveTime)
{
	printf("onMessage(): tid=%d received %zd bytes from connection [%s] at %s\n",
			muduo::CurrentThread::tid(),
			buf->readableBytes(),
			conn->name().c_str(),
			receiveTime.toFormattedString().c_str());
	//buf->retrieveAll();
	
}

int main(int argc, char* argv[])
{
	printf("main(): pid = %d\n", getpid());

	muduo::EventLoop loop;
	muduo::InetAddress serverAddr("localhost", 9981);
	muduo::TcpClient client(&loop, serverAddr);
		
	client.setConnectionCallback(onConnection);
	client.setMessageCallback(onMessage);
	client.enableRetry();
	client.connect();

	loop.loop();

	return 0;
}
