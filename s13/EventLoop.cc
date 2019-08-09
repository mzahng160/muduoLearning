#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include "TimerQueue.h"
#include "EPoller.h"

#include "logging/Logging.h"

#include <boost/bind.hpp>

#include <assert.h>
#include <sys/eventfd.h>
#include <signal.h>

using namespace muduo;

const int kPollTimeMs = 10000;

__thread EventLoop* t_loopThisThread = 0;

static int createEventfd()
{
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if(eventfd < 0)
	{
		LOG_ERROR << "Failed in eventfd";
		abort();
	}

	return evtfd;
}

class IgnoreSigPipe
{
public:
	IgnoreSigPipe()
	{
		::signal(SIGPIPE, SIG_IGN);
	}
	
};

IgnoreSigPipe initObj;

EventLoop::EventLoop()
	: wakeupFd_(createEventfd()),
	  wakeupChannel_(new Channel(this, wakeupFd_)),
	  looping_(false),
	  quit_(false),	  
	  callingPendingFunctors_(false),	  
	  threadId_(CurrentThread::tid()),
	  //poller_(new Poller(this)),
	  poller_(new EPoller(this)),
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

	wakeupChannel_->setReadCallback(
		boost::bind(&EventLoop::handleRead, this));
	wakeupChannel_->enableReading();
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
			(*it)->handleEvent(pollReturnTime_);
		}

		doPendingFunctors();
	}
	LOG_TRACE << "EventLoop " << this << " stop looping";
	looping_ = false;
}

void EventLoop::quit()
{
	quit_ = true;

	if(!isInLoopThread())
	{
		wakeup();
	}
}

void EventLoop::runInLoop(const Functor& cb)
{
	if(isInLoopThread())
	{
		cb();
	}
	else
	{
		queueInLoop(cb);
	}
}

void EventLoop::queueInLoop(const Functor& cb)
{
	{
		MutexLockGuard lock(mutex_);
		pendingFunctors_.push_back(cb);		
	}

	if(!isInLoopThread() || callingPendingFunctors_)
	{
		wakeup();
	}
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

void EventLoop::cancel(TimerId timerId)
{
	return timerQueue_->cancel(timerId);
}

void EventLoop::updateChannel(Channel* channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->updateChannel(channel);

}

void EventLoop::removeChannel(Channel* channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->removeChannel(channel);
}

void EventLoop::abortNotInLoopThread()
{
	LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
				<< " was created in threadId_ = " << threadId_
				<< ", current thread id = " << CurrentThread::tid();
}

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = ::write(wakeupFd_, &one, sizeof one);
	if(n != sizeof one)
	{
		LOG_ERROR << "EventLoop::wakeup() write " << n << " bytes";
	}

}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = ::read(wakeupFd_, &one, sizeof one);
	if(n != sizeof one)	
	{
		LOG_ERROR << "EventLoop::read() reads " << n << " bytes";
	}
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;

	callingPendingFunctors_ = true;
	
	{
		MutexLockGuard lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for(size_t i = 0; i < functors.size(); ++i)
	{
		functors[i]();
	}

	callingPendingFunctors_ = false;
}
