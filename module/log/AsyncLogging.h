#ifndef LOGMODULE_ASYNCLOGGING_H
#define LOGMODULE_ASYNCLOGGING_H

#include "../BlockQueue/BlockingQueue.h"
#include "../CountDownLatch/CountDownLatch.h"
#include "noncopyable.h"
#include "LogStream.h"
#include <atomic>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <thread>

namespace LogModule
{
	class AsyncLogging :noncopyable
	{
	public:
		AsyncLogging(const std::string& basename,
			off_t rollSize, int flushInterval = 3);
		~AsyncLogging()
		{
			if (running_)
			{
				stop();
			}			
		}

		void append(const char* logline, int len);

		void start()
		{
			running_ = true;
			latch_.wait();			
		}

		void stop() /*NO_THREAD_SAFETY_ANALYSIS*/
		{
			running_ = false;
			cond_.notify_all();
			thread_.join();
		}


	private:

		void threadFunc();

		typedef detail::FixedBuffer<LogModule::detail::kSmallBuffer> Buffer;
		typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
		typedef BufferVector::value_type BufferPtr;

		const int flushInterval_;		
		std::atomic<bool> running_;
		const std::string basename_;
		const off_t rollSize_;
		std::thread thread_;
		LogModule::CountDownLatch latch_;
		std::mutex mutex_;

		std::condition_variable cond_/*GUAREDE_BY(mutex_)*/;
		BufferPtr currentBuffer_ /*GUAREDE_BY(mutex_)*/;
		BufferPtr nextBuffer_ /*GUAREDE_BY(mutex_)*/;
		BufferVector buffers_ /*GUAREDE_BY(mutex_)*/;
	};

}

#endif // !LOGMODULE_ASYNCLOGGING_H