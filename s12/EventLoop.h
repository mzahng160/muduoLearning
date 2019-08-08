#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include "thread/Thread.h"
#include "thread/Mutex.h"
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

		typedef boost::function<void()> Functor;
		void runInLoop(const Functor& cb);
		void queueInLoop(const Functor& cb);


		TimerId runAt(const Timestamp& time, const TimerCallback& cb);
		TimerId runAfter(double delay, const TimerCallback& cb);
		TimerId runEvery(double interval, const TimerCallback& cb);

		void cancel(TimerId timerId);

		void wakeup();
		void updateChannel(Channel* channel);
		void removeChannel(Channel* channel);

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
		void handleRead();
		void doPendingFunctors();
		int wakeupFd_;
		boost::scoped_ptr<Channel> wakeupChannel_;

		bool looping_;
		bool quit_;
		bool callingPendingFunctors_;
		const pid_t threadId_;

		boost::scoped_ptr<Poller> poller_;
		boost::scoped_ptr<TimerQueue> timerQueue_;

		Timestamp pollReturnTime_;		

		typedef std::vector<Channel*> ChannelList;
		ChannelList activeChannels_;
		
		MutexLock mutex_;
		std::vector<Functor> pendingFunctors_ ;
	};
}

#endif	//MUDUO_NET_EVENTLOOP_H