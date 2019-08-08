#ifndef MUDUO_NET_TIMERQUEUE_H
#define MUDUO_NET_TIMERQUEUE_H

#include <set>
#include <vector>
#include <boost/noncopyable.hpp>

#include "datetime/Timestamp.h"
#include "thread/Mutex.h"
#include "Callbacks.h"
#include "Channel.h"

namespace muduo
{
	class EventLoop;
	class Timer;
	class TimerId;

	class TimerQueue : boost::noncopyable
	{
	public:
		TimerQueue(EventLoop* loop);
		~TimerQueue();

		TimerId addTimer(const TimerCallback& cb, Timestamp when,
			double interval);
		void cancel(TimerId timerId);

	private:
		typedef std::pair<Timestamp, Timer*> Entry;
		typedef std::set<Entry> TimerList;

		typedef std::pair<Timer*,  int64_t> ActiveTimer;
		typedef std::set<ActiveTimer> ActiveTimerSet;

		std::vector<Entry> getExpired(Timestamp now);
		void reset(const std::vector<Entry>& expired, Timestamp now);
		bool insert(Timer* timer);
		void addTimerInLoop(Timer* timer);
		void cancelInLoop(TimerId timerId);

		void handleRead();

		EventLoop* loop_;
		const int timerfd_;
		Channel timerfdChannel_;
		TimerList timers_;

		bool callingExpiredTimers_;	/*atomic*/
		ActiveTimerSet activeTimers_;
		ActiveTimerSet cancelingTimers_;
	};
	
}

#endif	//MUDUO_NET_TIMERQUEUE_H