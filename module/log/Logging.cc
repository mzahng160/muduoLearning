#include "Logging.h"
#include <stdlib.h>

namespace LogModule
{
	Logger::LogLevel initLogLevel()
	{
		if (::getenv("LOGMODULE_TRACE"))
			return Logger::TRACE;
		if (::getenv("LOGMODULE_DEBUG"))
			return Logger::DEBUG;
		else
			return Logger::INFO;
	}

	Logger::LogLevel g_logLevel = initLogLevel();

	Logger::~Logger()
	{

	}

	void Logger::setLogLevel(LogLevel level)
	{
		g_logLevel = level;
	}
}

using namespace LogModule;


