#include "EventLoopThread.h"
#include "EventLoop.h"

#include <boost/bind.hpp>

using namespace muduo;

EventLoopThread::EventLoopThread()
	:loop_(NULL),
	exiting(false),
	thread_(boost::bind(&EventLoopThread::threadFunc, this)),
	mutex_(),
	cond_(mutex_)
{

}
EventLoopThread::~EventLoopThread()
{
	exiting = true;
	loop_->quit();
	thread_.join();
}

EventLoop* EventLoopThread::startLoop()
{
	assert(!thread_.started());
	thread_.start();

	while(loop_ == NULL)
	{
		cond_.wait();
	}

	return loop_;	
}

void EventLoopThread::threadFunc()
{
	EventLoop loop;

	{
		MutexLockGuard lock(mutex_);
		loop_ = &loop;
		cond_.notify();
	}

	loop_->loop();
}