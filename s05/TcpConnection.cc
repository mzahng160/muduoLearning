#include "TcpConnection.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "logging/Logging.h"

#include <boost/bind.hpp>

using namespace muduo;

TcpConnection::TcpConnection(EventLoop* loop, 
				const std::string& name,
				int sockfd, 
				const InetAddress& localAddress,
				const InetAddress& peerAddr)
	:loop_(loop),
	 name_(name),
	 state_(kConnecting),
	 socket_(new Socket(sockfd)),
	 channel_(new Channel(loop, sockfd)),
	 localAddr_(localAddress),
	 peerAddr_(peerAddr)
{
	LOG_DEBUG << "TcpConnection::ctor[" << name_ << "] at " << this
				<< " fd =" << sockfd;

	channel_->setReadCallback(boost::bind(&TcpConnection::handleRead,this));
}

TcpConnection::~TcpConnection()
{}

void TcpConnection::connectEstablished()
{
	loop_->assertInLoopThread();
	assert(state_ == kConnecting);
	setState(kConnected);
	channel_->enableReading();

	connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead()
{
	char buf[65536];
	ssize_t n = ::read(channel_->fd(), buf, sizeof buf);
	messageCallback_(shared_from_this(), buf, n);
}