#include "TcpConnection.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "logging/Logging.h"
#include "SocketsOps.h"

#include <boost/bind.hpp>

#include <stdio.h>

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

	channel_->setReadCallback(
		boost::bind(&TcpConnection::handleRead,this, _1));
	channel_->setCloseCallback(
		boost::bind(&TcpConnection::handleClose, this));
	channel_->setErrorCallback(
		boost::bind(&TcpConnection::handleError, this));
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

void TcpConnection::connectDestroyed()
{
	loop_->assertInLoopThread();
	assert(state_ == kConnected);
	
	setState(kDisconnected);
	channel_->disableAll();

	connectionCallback_(shared_from_this());

	loop_->removeChannel(get_pointer(channel_));
	
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
	int savedErrno = 0;
	ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
	//::read(channel_->fd(), buf, sizeof buf);
	if(n > 0){		
		messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
	} else if(n == 0){
		handleClose();
	}else{
		errno = savedErrno;
		LOG_SYSERR << "TcpConnection::handleRead";
		handleError();
	}	
}

void TcpConnection::handleWrite()
{
}

void TcpConnection::handleClose()
{
	printf("TcpConnection::handleClose()!\n");

	loop_->assertInLoopThread();
	LOG_TRACE << "TcpConnection::handleClose state = " << state_;
	assert(state_ == kConnected);
	channel_->disableAll();
	closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
	int err = sockets::getSocketError(channel_->fd());
	LOG_ERROR << "TcpConnection::handleError [ " << name_
			<< " ] - SO_ERROR = " << err << " " << strerror_tl(err);
}
