#include <stdio.h>
#include "AsyncLogging.h"
#include "Logging.h"
#include <sys/resource.h>
#include <unistd.h>

off_t kRollSize = 500 * 1000 * 1000;

LogModule::AsyncLogging* g_asyncLog = nullptr;

void asyncOutput(const char* msg, int len)
{
	g_asyncLog->append(msg, len);
}

void bench(bool longLog)
{
	using namespace LogModule;

	using namespace std::chrono;

	LogModule::Logger::setOutput(asyncOutput);
	
	int cnt = 0;
	const int kBatch = 1000;
	std::string empty(" ");
	std::string longStr(3000, 'X');
	longStr += " ";

	for (int t = 0; t < 30; ++t)
	{
		steady_clock::time_point t_start = steady_clock::now();

		for (int i = 0; i < kBatch; ++i)
		{
			LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
				<< (longLog ? longStr : empty)
				<< cnt;
			++cnt;
		}

		steady_clock::time_point t_end = steady_clock::now();
		auto time_span = duration_cast<duration<double>>(t_end - t_start);
		printf("%d-----%f seconds\n", t, time_span.count());

		struct timespec ts = { 0, 500 * 1000 * 1000 };
		nanosleep(&ts, NULL);
	}
}

int main(int argc, char* argv[])
{
	{
		size_t kOneGB = 1000 * 1024 * 1024;
		rlimit rl = { 2 * kOneGB , 2 * kOneGB };
		setrlimit(RLIMIT_AS, &rl);
	}

	printf("async logging pid = %d\n", getpid());

	char name[256] = { 0 };
	strncpy(name, argv[0], sizeof name - 1);
	LogModule::AsyncLogging log(::basename(name), kRollSize);
	log.start();
	g_asyncLog = &log;

	bool longLog = argc > 1;
	bench(longLog);
}