#include "InetAddress.h"
#include "EventLoop.h"
#include "TcpClient.h"

#include <stdio.h>
#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

std::string message = "Hello";

void onConnection(const muduo::TcpConnectionPtr& conn)
{
	if(conn->connected())
	{
		printf("onConnection(): tid=%d new connection[%s] from %s\n",
			muduo::CurrentThread::tid(),
			conn->name().c_str(),
			conn->peerAddress().toHostPort().c_str());

		conn->send(message);
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
#if 0	
	printf("onMessage(): tid=%d received %zd bytes from connection [%s] at %s\n",
			muduo::CurrentThread::tid(),
			buf->readableBytes(),
			conn->name().c_str(),
			receiveTime.toFormattedString().c_str());
#endif	
	//printf("onMessage(): [%s]\n",  buf->retrieveAsString().c_str());
	
	if(message != buf->retrieveAsString())
	{
		printf("########## message:[%s] receive:[%s]\n",
			message.c_str(), buf->retrieveAsString().c_str());
	}

	conn->send(message);
}

void OnTimer(const muduo::TcpConnectionPtr& conn)
{
	printf("OnTimer(): sned message[%s]\n",  message.c_str());
	conn->send(message);
}

int main(int argc, char* argv[])
{
	printf("main(): pid = %d\n", getpid());

	muduo::EventLoop loop;
	muduo::InetAddress serverAddr("localhost", 9981);
/*
	muduo::TcpClient client(&loop, serverAddr);
		
	client.setConnectionCallback(onConnection);
	client.setMessageCallback(onMessage);
	client.enableRetry();
	client.connect();
*/


	boost::ptr_vector<muduo::TcpClient> v;
	for(int i=0; i < 1; ++i)
	{
		v.push_back(new muduo::TcpClient(&loop, serverAddr));
	}

	for(int i=0; i < 1; ++i)
	{
		v[i].setConnectionCallback(onConnection);
		v[i].setMessageCallback(onMessage);
		v[i].enableRetry();
		v[i].connect();
	}
	

	//loop.runEvery(3, OnTimer);
	//loop.runEvery(3, boost::bind(OnTimer, client.connection()));

	loop.loop();

	return 0;
}
