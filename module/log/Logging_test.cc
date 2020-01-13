#include "Logging.h"
#include <memory>
#include <unistd.h>
#include <string>
#include "../ThreadPool/ThreadPool.h"

std::unique_ptr<LogModule::Logger> g_log;

void logInThread()
{
}

int main()
{
	getppid();

	const int ThreadPoolCnt = 5;

	ThreadPool pool("pool");
	pool.start(ThreadPoolCnt);

	for (int i = 0; i < ThreadPoolCnt; ++i)
	{
		pool.run(logInThread);
	}
	return 0;
}