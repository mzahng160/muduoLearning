#include <stdio.h>
#include "EventLoop.h"
#include "InetAddress.h"

void newConnection(int sockfd, const muduo::InetAddress)
{

}

int main()
{
	printf("main(): pid = %d, tid = %d\n", 
		getpid(), muduo::CurrentThread::tid());

	muduo::InetAddress listenAddr(9981);
	muduo::EventLoop loop;

	muduo::Acceptor acceptor(&loop, listenAddr);
	acceptor.setNewConnectionCallback(newConnection);
	acceptor.listen();

	loop.loop();
}
