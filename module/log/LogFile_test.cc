#include "LogFile.h"
#include "Logging.h"

#include <unistd.h>

std::unique_ptr<LogModule::LogFile> g_logFile;

void outputFunc(const char* msg, int len)
{
	g_logFile->append(msg, len);
}

void flushFunc()
{
	printf("[%s flushFunc] flush!", __FILE__);
}

int main(int argc, char* argv[])
{
	using namespace LogModule;
	char name[256] = {0};
	strncpy(name, argv[0], sizeof name -1);
	g_logFile.reset(new LogModule::LogFile(::basename(name), 200*1000));
	LogModule::Logger::setOutput(outputFunc);
	LogModule::Logger::setFlush(flushFunc);

	std::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	for (int i = 0; i < 10000; ++i)
	{
		LOG_INFO << line << i;
		usleep(1000);
	}
}