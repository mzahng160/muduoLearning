#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include "TimerQueue.h"

#include "logging/Logging.h"

#include <assert.h>

using namespace muduo;

const int kPollTimeMs = 10000;

__thread EventLoop* t_loopThisThread = 0;

EventLoop::EventLoop()
	: looping_(false),
	  quit_(false),
	  threadId_(CurrentThread::tid()),
	  poller_(new Poller(this)),
	  timerQueue_(new TimerQueue(this))

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

	quit_ = false;

	while(!quit_)
	{
		activeChannels_.clear();
		pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);

		for(ChannelList::iterator it = activeChannels_.begin();
			it != activeChannels_.end(); ++it)
		{
			(*it)->handleEvent();
		}
	}

	LOG_TRACE << "EventLoop " << this << " stop looping";
	looping_ = false;
}

void EventLoop::quit()
{
	quit_ = true;
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{
	return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(), delay));
	return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::now(), interval));
	return timerQueue_->addTimer(cb, time, interval);
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