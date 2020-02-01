#include "AsyncLogging.h"
#include <functional>
#include <assert.h>
#include "LogFile.h"

using namespace LogModule;

AsyncLogging::AsyncLogging(const std::string& basename,
	off_t rollSize,
	int flushInterval)
	:flushInterval_(flushInterval),
	running_(false),
	basename_(basename),
	rollSize_(rollSize),
	latch_(1),
	currentBuffer_(new Buffer),
	nextBuffer_(new Buffer)

{
	currentBuffer_->bzero();
	nextBuffer_->bzero();
	buffers_.reserve(16);
	thread_ = std::thread(std::bind(&AsyncLogging::threadFunc, this));	
}

void AsyncLogging::append(const char* logline, int len)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (currentBuffer_->avail() > len)
	{
		currentBuffer_->append(logline, len);
	}
	else
	{
		buffers_.push_back(std::move(currentBuffer_));

		if (nextBuffer_)
		{
			currentBuffer_ = std::move(nextBuffer_);
		}
		else
		{
			currentBuffer_.reset(new Buffer);
		}
		currentBuffer_->append(logline, len);
		cond_.notify_one();
	}
}

void AsyncLogging::threadFunc()
{
	assert(true == running_);
	latch_.countDown();
	LogFile output(basename_, rollSize_, false);
	BufferPtr newBuffer1(new Buffer);
	BufferPtr newBuffer2(new Buffer);
	newBuffer1->bzero();
	newBuffer2->bzero();

	BufferVector buffersToWrite;
	buffersToWrite.reserve(16);
	while (running_)
	{
		assert(newBuffer1 && newBuffer1->length() == 0);
		assert(newBuffer2 && newBuffer2->length() == 0);
		assert(buffersToWrite.empty());

		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (buffers_.empty())
			{
				cond_.wait_for(lock, flushInterval_*1s);
			}
			buffers_.push_back(std::move(currentBuffer_));
			currentBuffer_ = std::move(newBuffer1);
			buffersToWrite.swap(buffers_);
			if (!nextBuffer_)
			{
				nextBuffer_ = std::move(newBuffer2);
			}
		}

		assert(!buffersToWrite.empty());

		if (buffersToWrite.size() > 25)
		{
			char timebuf[32];
			struct tm tm;
			time_t now = 0;
			now = time(nullptr);
			gmtime_r(&now, &tm);
			strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);

			char buf[256];
			snprintf(buf, sizeof buf, "Dropped log message at %s, %zd large buffers\n",
				timebuf, buffersToWrite.size() -2);
			fputs(buf, stderr);
			output.append(buf, static_cast<int>(strlen(buf)));
			buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
		}

		for (const auto& buffer : buffersToWrite)
		{
			output.append(buffer->data(), buffer->length());
		}

		if (buffersToWrite.size() > 2)
		{
			buffersToWrite.resize(2);
		}

		if (!newBuffer1)
		{
			assert(!buffersToWrite.empty());
			newBuffer1 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer1->reset();
		}

		if (!newBuffer2)
		{
			assert(!buffersToWrite.empty());
			newBuffer2 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer2->reset();
		}

		buffersToWrite.clear();
		output.flush();
	}
	output.flush();
}