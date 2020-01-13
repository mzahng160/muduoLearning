#include "ThreadPool.h"

#include <assert.h>
#include <mutex>
#include <exception>

ThreadPool::ThreadPool(const std::string& nameArg):
	mutex_(),
	name_(nameArg),
	maxQueueSize_(0),
	running_(false)
{

}
ThreadPool::~ThreadPool()
{
	if (running_)
	{
		stop();
	}
}

void ThreadPool::start(int numThreads)
{
	assert(threads_.empty());
	running_ = true;
	threads_.reserve(numThreads);

	for (int i = 0; i < numThreads; i++)
	{
		char id[32];
		snprintf(id, sizeof id, "%d", i);
		threads_.emplace_back(new std::thread(
			std::bind(&ThreadPool::runInThread, this), name_+ id));
	}

	if (numThreads == 0 && threadInitCallback_)
	{
		threadInitCallback_();
	}
}
void ThreadPool::stop()
{
	{
		std::lock_guard<std::mutex> lock(mutex_);
		running_ = false;
		notEmpty_.notify_all();
	}

	for (auto& thr : threads_)
	{
		thr->join();
	}
	
}

size_t ThreadPool::queueSize() const
{
	std::lock_guard<std::mutex> lock(mutex_);
	return queue_.size();
}

void ThreadPool::run(ThreadPool::Task task)
{
	if (threads_.empty())
	{
		task();
	}
	else
	{
		std::unique_lock<std::mutex> lock(mutex_);
		while (isFull())
		{
			notFull_.wait(lock);
		}

		assert(!isFull());

		queue_.push_back(std::move(task));
		notEmpty_.notify_one();
	}
}
bool ThreadPool::isFull() const
{
	return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

ThreadPool::Task ThreadPool::take()
{	
	std::unique_lock<std::mutex> lock(mutex_);
	while (queue_.empty() && running_)
	{
		notEmpty_.wait(lock);
	}

	Task task;
	if (!queue_.empty())
	{
		task = queue_.front();
		queue_.pop_front();
		if (maxQueueSize_ > 0)
		{
			notFull_.notify_one();
		}
	}
	return task;
}

void ThreadPool::runInThread()
{
	try {
		if (threadInitCallback_)
		{
			threadInitCallback_();
		}

		while (running_)
		{
			Task task(take());
			if (task)
			{
				task();
			}			
		}
	}
	catch (const std::exception& ex)
	{
		fprintf(stderr, "exception caught in threadPool %s\n", name_.c_str());
		fprintf(stderr, "reason:%s\n", ex.what());
		abort();
	}
	catch (...)
	{
		fprintf(stderr, "unknown exception caught in threadPool %s\n", name_.c_str());
		throw;
	}
}

