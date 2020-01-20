#include "LogFile.h"
#include "Logging.h"

std::unique_ptr<LogModule::LogFile> g_logFile;

void outputFunc()
{

}

void flushFunc()
{

}

int main()
{
	char name[256] = {0};
	strncpy(name, argv[0], sizeof name -1);
	g_logFile.reset(new LogModule::LogFile(::basename(name), 200*1000));
}