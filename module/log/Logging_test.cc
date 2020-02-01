#include "Logging.h"
#include <memory>
#include <unistd.h>
#include <string>
#include <chrono>
#include "../ThreadPool/ThreadPool.h"
#include "LogFile.h"

using namespace LogModule;

int g_total;
FILE* g_file;
std::unique_ptr<LogFile> g_logFile;

void dummyOutput(const char* msg, int len)
{
	g_total += len;
	if (g_file)
	{
		fwrite(msg, 1, len, g_file);
	}
	else if(g_logFile)
	{
		g_logFile->append(msg, len);
	}
}

void bench(const char* type)
{
	using namespace std::chrono;

	steady_clock::time_point t_start = steady_clock::now();

	Logger::setOutput(dummyOutput);
	g_total = 0;

	int n = 1000 * 1000;
	const bool kLongLog = false;
	std::string empty = " ";
	std::string longStr(3000, 'X');
	longStr += " ";

	for (int i = 0; i < n; ++i)
	{
		LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz"
			<< (kLongLog ? longStr : empty)
			<< i;
	}

	steady_clock::time_point t_end = steady_clock::now();
	//auto time_span = duration_cast<milliseconds>(t_end - t_start);
	auto time_span = duration_cast<duration<double>>(t_end - t_start);
	printf("%12s: %f seconds, %d bytes, %10.2f msg/s, %.2f MiB/s\n", 
		type, time_span.count(), g_total, n / time_span.count(), g_total/ time_span.count() /(1024*1024));
}

void logInThread()
{
	LOG_INFO << "logInThread";
	//printf("logInThread\n");
	usleep(1000*1000);
}

int main()
{
	getppid();

	const int ThreadPoolCnt = 0;

	ThreadPool pool("pool");
	pool.start(ThreadPoolCnt);

	for (int i = 0; i < ThreadPoolCnt; ++i)
	{
		pool.run(logInThread);
	}

	LOG_TRACE << "trace";
	LOG_DEBUG << "debug";
	LOG_WARN << "warning";
	LOG_ERROR << "error";

	LOG_INFO << sizeof(Logger);
	LOG_INFO << sizeof(LogStream);
	LOG_INFO << sizeof(Fmt);
	LOG_INFO << sizeof(LogStream::Buffer);	
	
	sleep(1);
	bench("nop");

	char buffer[64 * 1024];
#if 0
	g_file = fopen("/dev/null", "w");
	setbuf(g_file, buffer);
	bench("/dev/null");
	fclose(g_file);

	g_file = fopen("/tmp/log", "w");
	setbuf(g_file, buffer);
	bench("/tmp/log");
	fclose(g_file);
#endif
#if 0
	g_file = nullptr;
	g_logFile.reset(new LogModule::LogFile("test_log_st", 500*1000*1000, false));
	bench("test_log_st");


#endif
	g_logFile.reset(new LogModule::LogFile("test_log_mt", 500 * 1000 * 1000, true));
	bench("test_log_mt");
	g_logFile.reset();
	return 0;
}
