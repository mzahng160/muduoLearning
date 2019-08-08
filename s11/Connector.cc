#include "Connector.h"
#include "EventLoop.h"
#include "Channel.h"
#include "logging/Logging.h"
#include "SocketsOps.h"

#include <errno.h>
#include <boost/bind.hpp>
#include <stdio.h>

using namespace muduo;

const int Connector::kMaxRetryDelayMs;
//const int Connector::kInitRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& addr)
	: loop_(loop),
	  serverAddr_(addr),
	  connect_(false),	//atomic
	  state_(kDisconnected),		
	  retryDelayMs_(kInitRetryDelayMs)
{  
	LOG_DEBUG << "ctor[" << this << "]";
}

Connector::~Connector()
{
	LOG_DEBUG << "dtor[" << this << "]";
	loop_->cancel(timerId_);
	assert(!channel_);
}

void Connector::start()
{
	connect_ = true;
	loop_->runInLoop(boost::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop()
{
	loop_->assertInLoopThread();
	assert(kDisconnected == state_);

	if(connect_)
	{
		connect();
	}
	else
	{
		LOG_DEBUG << "do not connect";
	}
}

void Connector::connect()
{
	int sockfd = sockets::createNonblockingOrDie();
	int ret = sockets::connect(sockfd, serverAddr_.getSockAddrInet());
	int savedErrno = (0 == ret) ? 0: errno;

	switch(savedErrno)
	{
		case 0:
		case EINPROGRESS:
		case EINTR:
		case EISCONN:
			connecting(sockfd);
			LOG_DEBUG << "Connector::connect() connecting" ;
			break;

		case EAGAIN:
		case EADDRINUSE:
		case EADDRNOTAVAIL:
		case ECONNREFUSED:
		case ENETUNREACH:
			retry(sockfd);
			LOG_DEBUG << "Connector::connect() retry" ;
			break;

		case EACCES:
		case EPERM:
		case EAFNOSUPPORT:
		case EALREADY:
		case EBADF:
		case EFAULT:
		case ENOTSOCK:
			LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
			sockets::close(sockfd);
			break;

		default:
			LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
			sockets::close(sockfd);
			break;
	}
}

void Connector::restart()
{
	loop_->assertInLoopThread();
	setState(kDisconnected);
	retryDelayMs_ = kInitRetryDelayMs;	
	connect_ = true;

	startInLoop();
}

void Connector::stop()
{
	connect_ = false;
	loop_->cancel(timerId_);
}

void Connector::connecting(int sockfd)
{
	setState(kConnecting);
	assert(!channel_);
	channel_.reset(new Channel(loop_, sockfd));
	channel_->setWriteCallback(
		boost::bind(&Connector::handleWrite, this));

	channel_->setErrorCallback(
		boost::bind(&Connector::handleError, this));

	channel_->enableWriting();
}

int Connector::removeAndResetChannel()
{
	channel_->disableAll();
	loop_->removeChannel(get_pointer(channel_));
	int sockfd = channel_->fd();
	loop_->queueInLoop(boost::bind(&Connector::resetChannel, this));

	return sockfd;
}

void Connector::resetChannel()
{
	channel_.reset();
}

void Connector::handleWrite()
{
	LOG_TRACE << "Connector::handleWrite " << state_;

	if(state_ == kConnecting)
	{
		int sockfd = removeAndResetChannel();
		int err = sockets::getSocketError(sockfd);
		if(err)
		{
			LOG_WARN << "Connector::handleWrite - SO_ERROR = "
					<< err << " " << strerror_tl(err);
			retry(sockfd);
		}
		else if(sockets::isSelfConnect(sockfd))
		{
			LOG_WARN << "Connector::handleWrite - Self Connect";
			retry(sockfd);
		}
		else
		{
			setState(kConnected);
			if(connect_)
			{
				connectCallback_(sockfd);
			}
			else
			{
				sockets::close(sockfd);
			}
		}
	}
	else
	{
		assert(state_ == kDisconnected);
	}
}

void Connector::handleError()
{
	LOG_ERROR << "Connector::handleError";
	assert(state_ == kConnecting);

	int sockfd = removeAndResetChannel();
	int err = sockets::getSocketError(sockfd);
	LOG_TRACE << "SO_ERROR = " << err << " " << strerror_tl(err);
	retry(sockfd);
}

void Connector::retry(int sockfd)
{
	sockets::close(sockfd);
	setState(kDisconnected);

	if(connect_)
	{
		LOG_INFO << "Connector::retry - Retry connecting to "
				<< serverAddr_.toHostPort() << " in "
				<< retryDelayMs_ << " milliseconds.";

		timerId_ = loop_->runAfter(retryDelayMs_/1000.0, 
							boost::bind(&Connector::startInLoop, this));

		retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
		printf("retry connect delay %d ms\n", retryDelayMs_);
	}
	else
	{
		LOG_DEBUG << "do not connect";
	}
}