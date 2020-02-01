#ifndef LOGMODULE_COUNTDOWNLATCH_H
#define LOGMODULE_COUNTDOWNLATCH_H

#include "../noncopyable/noncopyable.h"
#include <mutex>
#include <condition_variable>

namespace LogModule
{
	class CountDownLatch : noncopyable
	{
	public:
		explicit CountDownLatch(int count);
		void wait();
		void countDown();
		int getCount() const;

	private:
		mutable std::mutex mutex_;
		std::condition_variable condition_;
		int count_;

	};
}

#endif //!LOGMODULE_COUNTDOWNLATCH_H