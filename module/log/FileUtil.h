#ifndef FILEUTIL_H
#define FILEUTIL_H

#include "noncopyable.h"
#include "StringPiece.h"

namespace FileUtil 
{
	class AppendFile : noncopyable
	{
	public:
		explicit AppendFile(StringArg filename);
		~AppendFile();

		void append(const char* logline, size_t len);
		void flush();
		off_t writetenBytes() const { return writtenBytes_; }

	private:
		FILE * fp_;
		char buffer_[64 * 1024];
		off_t writtenBytes_;
	};
}

#endif // !FILEUTIL_H
