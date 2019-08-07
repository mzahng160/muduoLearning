#include "Connector.h"
#include "EventLoop.h"
#include "Channel.h"
#include "logging/Logging.h"

#include <boost/bind.hpp>

using namespace muduo;

const int Connector::kMaxRetryDelayMs;
const int Connector::kInitRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& addr)
	: loop_(loop),
	  addr_(addr),
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

void Connector::restart()
{

}

void Connector::stop()
{

}

void Connector::startInLoop()
{

}

void Connector::connect()
{

}

void Connector::connecting(int sockfd)
{

}

void Connector::handleWrite()
{

}

void Connector::handleError()
{

}

void Connector::retry(int sockfd)
{

}

int Connector::removeAndResetChannel()
{
	return 0;
}

void Connector::resetChannel()
{

}