#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include "noncopyable.h"

namespace LogModule
{
	namespace detail 
	{
		const int kSmallBuffer = 4000;
		const int kLargeBuffer = 4000*1000;

		template<int SIZE>
		class FixedBuffer : noncopyable
		{
		public:
			
		};
	}

	class LogStream : noncopyable
	{
		typedef LogStream self;
	public:
		typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;
	};
}

#endif //LOGSTREAM_H