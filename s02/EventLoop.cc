#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"

#include "logging/Logging.h"

#include <assert.h>
#include <poll.h>

#include <iostream>

using namespace muduo;

const int kPollTimeMs = 10000;

__thread EventLoop* t_loopThisThread = 0;

EventLoop::EventLoop()
	: looping_(false),
	  quit_(false),
	  threadId_(CurrentThread::tid()),
	  poller_(new Poller(this))
{
	LOG_TRACE << "EventLoop created " << this << " in thread " << threadId_;
	if(t_loopThisThread)
	{
		LOG_FATAL << "Another EventLoop " << t_loopThisThread << 
			" exists in this thread " << threadId_;
	}
	else
		t_loopThisThread = this;
}

EventLoop::~EventLoop()
{
	assert(!looping_);
	t_loopThisThread = NULL;
}

void EventLoop::loop()
{
	assert(!looping_);
	assertInLoopThread();
	looping_ = true;

	while(!quit_)
	{
		activeChannels_.clear();
		poller_->poll(kPollTimeMs, &activeChannels_);

		for(ChannelList::iterator it = activeChannels_.begin();
			it != activeChannels_.end(); ++it)
		{
			(*it)->handleEvent();
		}
;
	}
	//::poll(NULL, 0, 5*1000);

	LOG_TRACE << "EventLoop " << this << " stop looping";
	looping_ = false;
}

void EventLoop::quit()
{
	quit_ = true;
}

void EventLoop::updateChannel(Channel* channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->updateChannel(channel);

}

void EventLoop::abortNotInLoopThread()
{
	LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
				<< " was created in threadId_ = " << threadId_
				<< ", current thread id = " << CurrentThread::tid();
}