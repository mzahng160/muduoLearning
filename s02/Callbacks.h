#ifndef MUDUO_NET_CALLBACKS_H
#define MUDUO_NET_CALLBACKS_H

#include <boost/function.hpp>

namespace muduo
{
	typedef boost::function<void()> TimerCallback;
}

#endif //MUDUO_NET_CALLBACKS_H