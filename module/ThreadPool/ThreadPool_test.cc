#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <sys/timeb.h>
#include <chrono>

void printThreadId()
{
	std::ostringstream oss;
	oss << std::this_thread::get_id();
	std::string stid = oss.str();
	unsigned long long tid = std::stoull(stid);

	printf("thread_id = %lld\n", tid);
}

void printString(const std::string& str)
{
	//std::cout << str << std::endl;
	printf("%s\n", str.c_str());
	usleep(100*1000);
}

void test(int maxSize)
{
	printf("Test ThreadPool with max queue size = %d\n", maxSize);
	ThreadPool pool("MainThreadPool");
	pool.setMaxQueueSize(maxSize);
	pool.start(5);

	printf("adding\n");
	pool.run(printThreadId);
	pool.run(printThreadId);

	for (int i = 0; i < 100; ++i)
	{
		char buf[32];
		snprintf(buf, sizeof buf, "task %d", i);
		pool.run(std::bind(printString, std::string(buf)));
	}

	printf("Done\n");

	pool.stop();
}

int main()
{
	struct  timeb timeStart;
	ftime(&timeStart);

	//test(0);
	//test(5);

	test(50);
	struct  timeb timeEnd;
	ftime(&timeEnd);

	std::cout << "cost "
		<< (timeEnd.time - timeStart.time) * 1000 + timeEnd.millitm - timeStart.millitm << "ms"
		<< std::endl;
}
