#ifndef MUDUO_NET_TIMER_H
#define MUDUO_NET_TIMER_H

#include <boost/noncopyable.hpp>

#include "datetime/Timestamp.h"
#include "Callbacks.h"

namespace muduo
{
	class Timer : boost::noncopyable
	{
	public:
		Timer(const TimerCallback& cb, Timestamp when, double interval)
			:callback_(cb),
			 expiration_(when),
			 interval_(interval),
			 repeat_(interval > 0.0)
		{}

		void run() const
		{
			callback_();			
		}

		void restart(Timestamp now);

		Timestamp expiration() const { return expiration_; }
		bool repeat() const { return repeat_; }		

	private:
		const TimerCallback callback_;
		Timestamp expiration_;
		const double interval_;
		const bool repeat_;
	};
}

#endif	//MUDUO_NET_TIMER_H