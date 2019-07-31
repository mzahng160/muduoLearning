#include "Socket.h"
#include "SocketsOps.h"
#include "InetAddress.h"

#include <strings.h>

using namespace muduo;

void Socket::bindAddress(const InetAddress& localaddr)
{

}

void Socket::listen()
{
	sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peeraddr)
{
	struct sockaddr_in addr;
	bzero(&addr, sizeof addr);
	int connfd = sockets::accept(sockfd_, &addr);
	if(connfd >= 0)
	{
		peeraddr->setSockAddrInet(addr);
	}

	return connfd;
}

void Socket::setReuseAddr(bool on)
{

}