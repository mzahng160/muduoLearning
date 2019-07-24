#include "TimerQueue.h"
#include "logging/Logging.h"
#include "EventLoop.h"
#include "Timer.h"
#include "TimerId.h"

#include <sys/timerfd.h>
#include <boost/bind.hpp>

namespace muduo
{
	namespace detail
	{
		int createTimerfd()
		{
			int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

			if(timerfd < 0)
			{
				LOG_SYSFATAL << "Failed in timerfd_create";
			}
			return timerfd;
		}

		struct timespec howMuchTimeFromNow(Timestamp when)
		{
			int64_t microseconds = when.microSecondsSinceEpoch()
								- Timestamp::now().microSecondsSinceEpoch();
			if(microseconds < 100)
			{
				microseconds = 100;
			}

			struct timespec ts;
			ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
			ts.tv_nsec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond * 1000);
			
			return ts;
		}

		void readTimerfd(int timerfd, Timestamp now)
		{
			uint64_t howmany;
			ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
			LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();

			if(n != sizeof howmany)
				LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
		}

		void resetTimerfd(int timerfd, Timestamp expiration)
		{
			struct itimerspec newValue;
			struct itimerspec oldValue;
			bzero(&newValue, sizeof newValue);
			bzero(&oldValue, sizeof oldValue);

			newValue.it_value = howMuchTimeFromNow(expiration);
			int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);

			if(ret)
			{
				LOG_SYSERR << "timerfd_setrtime()";
			}
		}
	}
}

using namespace muduo;
using namespace muduo::detail;

TimerQueue::TimerQueue(EventLoop* loop)
	:loop_(loop),
	 timerfd_(createTimerfd()),
	 timerfdChannel_(loop, timerfd_),
	 timers_()
{
	timerfdChannel_.setReadCallback(boost::bind(&TimerQueue::handleRead,this));
	timerfdChannel_.enableReading();

}
TimerQueue::~TimerQueue()
{
	::close(timerfd_);
	for(TimerList::iterator it = timers_.begin();
			it != timers_.end(); ++it)
	{
		delete it->second;
	}
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when,
	double interval)
{
	Timer* timer = new Timer(cb, when, interval);
	loop_->assertInLoopThread();
	bool earliestChanged = insert(timer);
	if(earliestChanged)
	{
		resetTimerfd(timerfd_, timer->expiration());
	}

	return TimerId(timer);
}

void TimerQueue::handleRead()
{
	loop_->assertInLoopThread();
	Timestamp now(Timestamp::now());
	readTimerfd(timerfd_, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
	return 
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{

}

bool TimerQueue::insert(Timer* timer)
{

}