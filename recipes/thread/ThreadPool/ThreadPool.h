#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <functional>
#include <string>
#include <deque>
#include <vector>
#include <thread>
#include <condition_variable>

class ThreadPool 
{
public:
	typedef std::function<void()> Task;

	explicit ThreadPool(const std::string& nameArg = std::string("ThreadPool"));
	~ThreadPool();

	void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }
	void setThreadInitCallback(const Task& cb)
	{
		threadInitCallback_ = cb;
	}

	void start(int numThreads);
	void stop();

	const std::string& name() const
	{
		return name_; 
	}

	size_t queueSize() const;
	void run(Task t);

private:
	bool isFull() const; //REQUIRES(mutex_);
	void runInThread();
	Task take();

	mutable std::mutex mutex_;

	std::condition_variable notEmpty_; //GUARDED_BY(mutex_);
	std::condition_variable notFull_; // GUARDED_BY(mutex_);

	std::string name_;
	Task threadInitCallback_;

	std::vector <std::unique_ptr<std::thread>> threads_;	
	std::deque<Task> queue_; // GUARDED_BY(mutex_);
	size_t maxQueueSize_;
	bool running_;
};

#endif	//THREADPOOL_H