#ifndef LOGGING_H
#define LOGGING_H

#include <string.h>
#include "LogStream.h"

namespace LogModule
{
	class Logger {
	public:
		enum LogLevel
		{
			TRACE,
			DEBUG,
			INFO,
			WARN,
			ERROR,
			FATAL,
			NUM_LOG_LEVELS,
		};

		class SourceFile
		{
		public:
			template<int N>
			SourceFile(const char(&arr)[N])
				:data_(arr),
				 size_(N-1)
			{
				const char* slash = strrchr(data_, '/');
				if (slash)
				{
					data_ = slash + 1;
					size_ -= static_cast<int>(data_ - arr);
				}
			}

			explicit SourceFile(const char* filename)
				:data_(filename)
			{
				const char* slash = strrchr(filename, '/');
				if (slash)
				{
					data_ = slash + 1;					
				}
				size_ -= static_cast<int>(strlen(data_));
			}

			const char* data_;
			int size_;
		};

		Logger(SourceFile file, int line);
		Logger(SourceFile file, int line, LogLevel level);
		Logger(SourceFile file, int line, LogLevel level, const char* func);
		Logger(SourceFile file, int line, bool toAbort);
		~Logger();

		LogStream& stream() { return impl_.stream_; }		

		static LogLevel logLevel();
		static void setLogLevel(LogLevel level);

	private:
		class Impl 
		{
		public:
			LogStream stream_;
		};

		Impl impl_;
	};


	extern Logger::LogLevel g_logLevel;
	inline Logger::LogLevel Logger::logLevel()
	{
		return g_logLevel;
	}
#define LOG_INFO if(Logger::logLevel() <= Logger::INFO) \
			Logger(__FILE__, __LINE__).stream()
}

#endif // !LOGGING_H
