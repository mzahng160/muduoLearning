#include "SocketsOps.h"
#include "logging/Logging.h"

#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
//#include <strings.h>
//#include <unistd.h>
#include <stdio.h>

using namespace muduo;

namespace 
{

	typedef struct sockaddr SA;

	const SA* sockadd_cast(const struct sockaddr_in* addr)
	{
		return static_cast<const SA*>(implicit_cast<const void*>(addr));
	}

	SA* sockadd_cast (struct sockaddr_in* addr)
	{
		return static_cast<SA*>(implicit_cast<void*>(addr));
	}

	void setNonBlockAndCloseOnExec(int sockfd)
	{
		int flags = ::fcntl(sockfd, F_GETFL, 0);
		flags |= O_NONBLOCK;
		/*int ret = */::fcntl(sockfd, F_SETFL, flags);

		flags = ::fcntl(sockfd, F_GETFD, 0);
		flags |= FD_CLOEXEC;
		/*ret = */::fcntl(sockfd, F_SETFD, flags);
	}
}

int sockets::createNonblockingOrDie()
{
#if VALGRIND
	int sockfd = ::socket(AD_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd < 0)
		LOG_SYSFATAL << "sockets::createNonblockingOrDie";
	setNonBlockAndCloseOnExec(sockfd);	
#else
	int sockfd = ::socket(AF_INET, 
				SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,
				IPPROTO_TCP);
	if(sockfd < 0)
		LOG_SYSFATAL << "sockets::createNonblockingOrDie";
#endif
	return sockfd;
}

void sockets::bindOrDie(int sockfd, const struct sockaddr_in& addr)
{
	int ret = ::bind(sockfd, sockadd_cast(&addr), sizeof addr);
	if(ret < 0)
		LOG_SYSFATAL << "sockets::bindOrDie";
}

void sockets::listenOrDie(int sockfd)
{
	int ret = ::listen(sockfd, SOMAXCONN);
	if(ret < 0)
		LOG_SYSFATAL << "sockrts::listenOrDie";
}

int sockets::accept(int sockfd, struct sockaddr_in* addr)
{
	socklen_t addrlen = sizeof *addr;
#if VALGRIND
	int connfd = ::accept(sockfd, sockadd_cast(addr), &addrlen);
	setNonBlockAndCloseOnExec(connfd);
#else
	int connfd = ::accept4(sockfd, sockadd_cast(addr), 
		&addrlen, SOCK_NONBLOCK|SOCK_CLOEXEC);
#endif
	if(connfd < 0)
	{
		int savedErrno = errno;
		LOG_SYSERR << "sockrts::accept";
		switch(savedErrno)
		{
			case EAGAIN:
			case ECONNABORTED:
			case EINTR:
			case EPROTO:
			case EPERM:
			case EMFILE:
				errno = savedErrno;
				break;
			case EBADF:
			case EFAULT:
			case EINVAL:
			case ENFILE:
			case ENOBUFS:
			case ENOMEM:
			case ENOTSOCK:
			case EOPNOTSUPP:
				LOG_FATAL << "unexpected error of ::accept " << savedErrno;
				break;
			default:
				LOG_FATAL << "unkonow error of ::accept " << savedErrno;
				break;
		}
	}

	return connfd;
}

void sockets::close(int sockfd)
{
	if(::close(sockfd) < 0)
		LOG_SYSERR << "sockets::close";
}

void sockets::toHostPort(char* buf, size_t size,
			const struct sockaddr_in& addr)
{
	char host[INET_ADDRSTRLEN] = "INVALID";
	::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof host);
	uint16_t port = sockets::networkToHost16(addr.sin_port);
	snprintf(buf, size, "%s:%u", host, port);
}
void sockets::fromHostPort(const char* ip, uint16_t port,
					struct sockaddr_in* addr)
{
	addr->sin_family = AF_INET;
	addr->sin_port = hostToNetwork16(port);
	if(::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
		LOG_SYSERR << "sockets::fromHostPort";
}
