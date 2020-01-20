#ifndef LOGMODULE_LOGFILE_H
#define LOGMODULE_LOGFILE_H

#include "noncopyable.h"
#include <string>
#include <mutex>
#include <memory>

namespace FileUtil
{
	class AppendFile;
}

namespace LogModule
{
	class LogFile : noncopyable
	{
	public:
		LogFile(const std::string& basename, 
			off_t roolSize, 
			bool threadSafe = true,
			int flushInterval = 3,
			int checkEveryN = 1024);
		~LogFile();

		void append(const char* logline, int len);
		void flush();
		bool rollFile();
	private:
		void append_unlocked(const char* logline, int len);
		static std::string getLogFileName(const std::string& basename, time_t* now);

		std::string basename_;
		const off_t rollSize_;
		const int flushInterval_;
		const int checkEveryN_;
		bool threadSafe_;
		int count_;

		std::mutex mutex_;
		time_t startOfPeriod_;
		time_t lastRoll_;
		time_t lastFlush_;

		std::unique_ptr<FileUtil::AppendFile> file_;

		const static int kRollPreSeconds_ = 60 * 60 * 24;
	};
}

#endif // !LOGMODULE_LOGFILE_H