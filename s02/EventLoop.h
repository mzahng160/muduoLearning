#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include "thread/Thread.h"

#include <boost/scoped_ptr.hpp>
#include <vector>

namespace muduo
{
	class Channel;
	class Poller;

	class EventLoop : boost::noncopyable
	{
	public:
		EventLoop();
		~EventLoop();

		void loop();
		void quit();

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

		typedef std::vector<Channel*> ChannelList;
		ChannelList activeChannels_;
	};
}

#endif	//MUDUO_NET_EVENTLOOP_H