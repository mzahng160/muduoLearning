#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include "thread/Thread.h"
#include "TimerId.h"
#include "datetime/Timestamp.h"
#include "Callbacks.h"

#include <boost/scoped_ptr.hpp>
#include <vector>

namespace muduo
{
	class Channel;
	class Poller;
	class TimerQueue;

	class EventLoop : boost::noncopyable
	{
	public:
		EventLoop();
		~EventLoop();

		void loop();
		void quit();

		Timestamp pollReturnTime() const {return  pollReturnTime_;}

		TimerId runAt(const Timestamp& time, const TimerCallback& cb);
		TimerId runAfter(double delay, const TimerCallback& cb);
		TimerId runEvery(double interval, const TimerCallback& cb);

		void updateChannel(Channel* channel);

		void assertInLoopThread()
		{
			if(!isInLoopThread())
			{
				abortNotInLoopThread();
			}
		}

		bool isInLoopThread() const { return threadId_ == CurrentThread::tid();}

	private:
		void abortNotInLoopThread();

		bool looping_;
		bool quit_;
		const pid_t threadId_;

		boost::scoped_ptr<Poller> poller_;
		boost::scoped_ptr<TimerQueue> timerQueue_;

		Timestamp pollReturnTime_;		

		typedef std::vector<Channel*> ChannelList;
		ChannelList activeChannels_;
	};
}

#endif	//MUDUO_NET_EVENTLOOP_H