#include "Channel.h"
#include "EventLoop.h"
#include "logging/Logging.h"

#include <sstream>

#include <poll.h>

using namespace muduo;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
	: loop_(loop)
	, fd_(fd)
	, events_(0)
	, revents_(0)
	, index_(-1)
	, eventHanling_(false)
{
}

Channel::~Channel()
{
	assert(!eventHanling_);
}

void Channel::update()
{
	loop_->updateChannel(this);
}

void Channel::handleEvent()
{
	eventHanling_ = true;

	if(revents_ & POLLNVAL)
	{
		LOG_WARN << "Channel::handle_event() POLLNVAL";
	}

	if((revents_ & POLLHUP) && !(revents_ & POLLIN))
	{
		LOG_WARN << "Channel::handle_event() POLLHUP";
		if(closeCallback_) closeCallback_();
		printf("Channel closeCallback_()\n");
	}
	if(revents_ & (POLLERR | POLLNVAL))
	{
		if(errorCallback_) errorCallback_();
	}
	if(revents_ & (POLLIN | POLLPRI | POLLRDHUP))
	{
		if(readCallback_) readCallback_();
	}

	if(revents_ & POLLOUT)
	{
		if(writeCallback_) writeCallback_();
	}

	eventHanling_ = false;
}