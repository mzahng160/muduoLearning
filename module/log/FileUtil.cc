#include "FileUtil.h"
#include "Logging.h"
#include <assert.h>

using namespace FileUtil;

AppendFile::AppendFile(StringArg filename)
	:fp_(::fopen(filename.c_str(), "ae"))
{
	assert(fp_);
	::setbuffer(fp_, buffer_, sizeof buffer_);

}

AppendFile::~AppendFile()
{
	::fclose(fp_);
}


void AppendFile::append(const char* logline, size_t len)
{
	//size_t n = write(logline, len);
	size_t n = fwrite_unlocked(logline, 1, len, fp_);
	size_t remain = len - n;

	while (remain > 0)
	{
		size_t x = fwrite_unlocked(logline+n, 1, remain, fp_);
		if (x == 0)
		{
			int err = ferror(fp_);
			if (err)
			{
				fprintf(stderr, "AppendFile::append() failed %s\n", LogModule::strerror_tl(err));
			}
		}
		n += x;
		remain = len - n;
	}
	writtenBytes_ += len;
}

void AppendFile::flush()
{
	::fflush(fp_);
}