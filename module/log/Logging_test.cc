#include "Logging.h"
#include <memory>
#include <unistd.h>
#include <string>
#include "../ThreadPool/ThreadPool.h"

using namespace LogModule;

void logInThread()
{
	LOG_INFO << "logInThread";
	//printf("logInThread\n");
	usleep(1000*1000);
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