#include <stdio.h>
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"
#include "Acceptor.h"

void newConnection(int sockfd, const muduo::InetAddress peerAddr)
{
	printf("newConnection(): accept a new connection from %s\n",
		peerAddr.toHostPort().c_str());
	
	std::string time(muduo::Timestamp::now().toFormattedString());	
	::write(sockfd, time.c_str(), time.size());
	//::write(sockfd, "How are you?\n", 13);
	muduo::sockets::close(sockfd);
}

void newConnection_2(int sockfd, const muduo::InetAddress& peerAddr)
{
  printf("newConnection(): accepted a new connection from %s\n",
         peerAddr.toHostPort().c_str());
  ::write(sockfd, "How are you?\n", 13);
  muduo::sockets::close(sockfd);
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  muduo::EventLoop loop;

  muduo::InetAddress listenAddr(9981);
  muduo::Acceptor acceptor(&loop, listenAddr);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();

  muduo::InetAddress listenAddr_2(9982);
  muduo::Acceptor acceptor_2(&loop, listenAddr_2);
  acceptor_2.setNewConnectionCallback(newConnection_2);
  acceptor_2.listen();

  loop.loop();
}
