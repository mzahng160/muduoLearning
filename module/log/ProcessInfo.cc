#include "ProcessInfo.h"
#include <unistd.h>

pid_t ProcessInfo::pid()
{
	return ::getpid();
}
std::string ProcessInfo::hostname()
{
	char buf[256];
	if (0 == ::gethostname(buf, sizeof buf))
	{
		buf[sizeof(buf) - 1] = '\0';
		return buf;
	}
	else
	{
		return "unknowhost";
	}
}