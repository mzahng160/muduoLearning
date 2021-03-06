#include "Logging.h"
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <string>
#include <string.h>
#include <assert.h>
#include <sstream>
#include <time.h>

__thread char t_errnobuf[512];

namespace LogModule
{
	__thread char t_errorbuf[512];
	const char* strerror_tl(int savedErrno)
	{
		return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
	}

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

	const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
	{
		"TRACE",
		"DEBUG",
		"INFO",
		"WARN",
		"ERROR",
		"FATAL",
	};

	class T
	{
	public:
		T(const char* str, size_t len)
			:str_(str),
			len_(len)
		{
			assert(len == strlen(str));
		}

		const char* str_;
		const unsigned len_;
	};

	void defaultOutput(const char* msg, int len)
	{
		size_t n = fwrite(msg, 1, len, stdout);
		assert(n == len);
	}

	void defaultFlush()
	{
		fflush(stdout);
	}

	Logger::OutputFunc g_output = defaultOutput;
	Logger::FlushFunc g_flush = defaultFlush;

	Logger::Impl::Impl(LogLevel level, int old_errno, const SourceFile& file, int line)
		:stream_(),
		//time_(Timestamp::now()),
		level_(level),
		line_(line),
		basename_(file)
	{
		formatTime();
		//CurrentThread::id();
		std::thread::id id = std::this_thread::get_id();
		std::ostringstream oss;
		oss << std::this_thread::get_id();
		std::string stid = oss.str();

		stream_ << stid;
		stream_ << LogLevelName[level];
		stream_ << '-';
		if (old_errno != 0)
		{
			;//stream_ <<

		}

	}

	void Logger::Impl::formatTime()
	{
		time_t result;
		char str[26];
		char buf[26];
		time(&result);
		ctime_r(&result, str);		
		snprintf(buf,25, "%s", str);
		stream_ << buf;
		stream_ << '-';
	}

	void Logger::Impl::finish()
	{		
		stream_ << '-' << basename_.data_ << ':' << line_ << '\n';
	}

	Logger::~Logger()
	{
		impl_.finish();
		const LogStream::Buffer& buf(stream().buffer());
		g_output(buf.data(), buf.length());
		if (impl_.level_ == FATAL)
		{
			g_flush();
			abort();
		}
	}

	void Logger::setLogLevel(LogLevel level)
	{
		g_logLevel = level;
	}

	Logger::Logger(SourceFile file, int line)
		:impl_(INFO, 0, file, line)
	{
		
	}

	Logger::Logger(SourceFile file, int line, LogLevel level)
		:impl_(level, 0, file, line)
	{
	
	}

	Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
		: impl_(level, 0, file, line)
	{
		impl_.stream_ << func << ' ';
	}

	Logger::Logger(SourceFile file, int line, bool toAbort)
		: impl_(toAbort? FATAL: ERROR, errno, file, line)
	{
	
	}

	void Logger::setOutput(OutputFunc out)
	{
		g_output = out;
	}

	void Logger::setFlush(FlushFunc flush)
	{
		g_flush = flush;
	}

}

using namespace LogModule;


