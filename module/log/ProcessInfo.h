#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <sys/types.h>
#include <string>

namespace ProcessInfo
{
	pid_t pid();
	std::string hostname();
}

#endif // !PROCESSINFO_H
