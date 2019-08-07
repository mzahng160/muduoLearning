#ifndef MUDUO_NET_TIMERID_H
#define MUDUO_NET_TIMERID_H

#include "datetime/copyable.h"

namespace muduo
{
	class Timer;
	class TimerId : public muduo::copyable
	{
	public:
		explicit TimerId(Timer* timer = NULL, int64_t seq = 0)
			: value_(timer),
			  sequence_(seq)
		{
		}		
		
		friend class TimerQueue;

	private:
		Timer* value_;
		int64_t sequence_;

	};
}

#endif //MUDUO_NET_TIMERID_H
