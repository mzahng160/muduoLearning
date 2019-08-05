#ifndef MUDUO_NET_POLLER_H
#define MUDUO_NET_POLLER_H

#include "EventLoop.h"
#include "datetime/Timestamp.h"

#include <vector>
#include <map>

struct pollfd;

namespace muduo
{

	class Channel;

	class Poller : boost::noncopyable
	{
	public:		
		Poller(EventLoop* loop);
		~Poller();

		typedef std::vector<Channel*> ChannelList;
		Timestamp poll(int timeoutMs, ChannelList* activeChannels);
		void updateChannel(Channel* channel);
		void removeChannel(Channel* channel);
		void assertInLoopThread() {ownerLoop_->assertInLoopThread();}

	private:
		void fillActiveChannels(int numEvents, 
								ChannelList* activeChannels) const;
		typedef std::vector<struct pollfd> PollFdList;
		typedef std::map<int, Channel*> ChannelMap;

	private:
		EventLoop* ownerLoop_;
		PollFdList pollfds_;
		ChannelMap channels_;
	};
}

#endif//MUDUO_NET_POLLER_H