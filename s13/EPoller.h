#ifndef MUDUO_NET_EPOLLER_H
#define MUDUO_NET_EPOLLER_H

#include "EventLoop.h"
#include "datetime/Timestamp.h"
#include <map>
#include <vector>

struct epoll_event;

namespace muduo
{
	class Channel;

	class EPoller : boost::noncopyable
	{
	public:
		typedef std::vector<Channel*> ChannelList;

		EPoller(EventLoop* loop);
		~EPoller();
		
		Timestamp poll(int timeoutMs, ChannelList* activeChannels);
		void updateChannel(Channel* channel);
		void removeChannel(Channel* channel);

		void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }

	private:
		static const int kInitEventListSize = 16;

		void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
		void update(int operation, Channel* channel);

		typedef std::vector<struct epoll_event> EventList;
		typedef std::map<int, Channel*> ChannelMap;

		EventLoop* ownerLoop_;
		int epollfd_;
		EventList events_;
		ChannelMap channels_;
	};
}

#endif