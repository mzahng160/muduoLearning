#include "TcpConnection.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "logging/Logging.h"
#include "SocketsOps.h"

#include <boost/bind.hpp>
#include <functional>

#include <stdio.h>
#include <errno.h>

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
{
	LOG_DEBUG << "TcpConnection::dtor[" << name_ << "] at "
			<< this << " fd=" <<channel_->fd();
}

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
	assert(state_ == kConnected || state_ == kDisconnecting);
	
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
	loop_->assertInLoopThread();
	if(channel_->isWriting()){
		ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), 
								outputBuffer_.readableBytes());
		if(n > 0)
		{
			outputBuffer_.retrieve(n);
			if(0 == outputBuffer_.readableBytes())
			{
				channel_->disableWriting();
				
				if(writeCompleteCallback_)
				{
					loop_->queueInLoop(
						boost::bind(writeCompleteCallback_, shared_from_this()));
				}									
				if(state_ == kDisconnecting)
					shutdownInLoop();				
			}
			else			
				LOG_TRACE << "I am going write more";			
		}
		else		
			LOG_SYSERR << "TcpConnection::handleWrite";		
	}
	else
		LOG_TRACE << "Connection is down, no more writing";	
}

void TcpConnection::send(const std::string& message)
{
	if(kConnected == state_){
		if(loop_->isInLoopThread())
			sendInLoop(message);
		else
			loop_->runInLoop(
				boost::bind(&TcpConnection::sendInLoop, this, message));
	}
}

void TcpConnection::send(Buffer* buf)
{
	if(kConnected == state_)
	{
		if(loop_->isInLoopThread())
		{
			sendInLoop(buf->peek(), buf->readableBytes());
			buf->retrieveAll();
		}
		else
		{
			std::string message(buf->retrieveAllAsString());
			loop_->runInLoop(
				boost::bind(&TcpConnection::sendInLoop, this, message));
		}
	}
}

void TcpConnection::sendInLoop(const std::string& message)
{
	loop_->assertInLoopThread();
	ssize_t nwrote = 0;

	if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
	{
		nwrote = ::write(channel_->fd(), message.data(), message.size());
		if(nwrote >= 0)
		{
			if(implicit_cast<size_t>(nwrote) < message.size())
			{
				LOG_TRACE << "I am going to write more data";
			}
			else if(writeCompleteCallback_)
			{
				loop_->queueInLoop(
					boost::bind(writeCompleteCallback_, shared_from_this()));
			}
		}
		else
		{
			nwrote = 0;
			if(errno != EWOULDBLOCK)
			{
				LOG_SYSERR << "TcpConnection::sendInLoop";
			}				
		}
	}

	assert(nwrote >= 0);

	if(implicit_cast<size_t>(nwrote) < message.size()){
		outputBuffer_.append(message.data()+nwrote, message.size()-nwrote);
		if(!channel_->isWriting()){
			channel_->enableWriting();
		}
	}
	std::string str(outputBuffer_.peek(), outputBuffer_.readableBytes());	
}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
	loop_->assertInLoopThread();
	ssize_t nwrote = 0;
	size_t remaining = len;
	bool faultError = false;
	if(kDisconnected ==  state_)
	{
		LOG_WARN << "kDisconnected, give up writing";
		return;
	}

	if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
	{
		nwrote = ::write(channel_->fd(), data, len);
		if(nwrote >= 0)
		{
			remaining = len - nwrote;

			if(remaining == 0 && writeCompleteCallback_)
			{
				loop_->queueInLoop(
					std::bind(writeCompleteCallback_, shared_from_this()));
			}		
		}
		else
		{
			nwrote = 0;
			if(errno != EWOULDBLOCK)
			{
				LOG_SYSERR << "TcpConnection::sendInLoop";
				if(errno == EPIPE || errno == ECONNRESET)
					faultError = true;
			}
		}
	}

	assert(remaining <= len);

	if(!faultError && remaining > 0)
	{
		size_t oldlen = outputBuffer_.readableBytes();
		if(oldlen + remaining >= highWaterMark_
			&& oldlen < highWaterMark_
			&& highWaterMarkCallback_)
		{
			loop_->queueInLoop(std::bind(highWaterMarkCallback_,
						shared_from_this(), oldlen + remaining));
			outputBuffer_.append(
				static_cast<const char*>(data)+nwrote, remaining);
			if(!channel_->isWriting())
			{
				channel_->enableWriting();
			}
		}
	}
}

void TcpConnection::shutdown()
{
	if(kConnected == state_)
	{
		setState(kDisconnecting);
		loop_->runInLoop(boost::bind(&TcpConnection::shutdownInLoop, this));
	}
}

void TcpConnection::setTcpNoDelay(bool on)
{
	socket_->setTcpNoDelay(on);
}

void TcpConnection::shutdownInLoop()
{
	loop_->assertInLoopThread();
	if(!channel_->isWriting())
	{
		socket_->shutdownWrite();
	}
}

void TcpConnection::handleClose()
{
	printf("TcpConnection::handleClose()!\n");

	loop_->assertInLoopThread();
	LOG_TRACE << "TcpConnection::handleClose state = " << state_;
	assert(state_ == kConnected || state_ == kDisconnecting);
	channel_->disableAll();
	closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
	int err = sockets::getSocketError(channel_->fd());
	LOG_ERROR << "TcpConnection::handleError [ " << name_
			<< " ] - SO_ERROR = " << err << " " << strerror_tl(err);
}