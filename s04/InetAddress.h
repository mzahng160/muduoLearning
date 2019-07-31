#ifndef MUDUO_NET_INETADDRESS_H
#define MUDUO_NET_INETADDRESS_H

#include "datetime/copyable.h"

#include <string>
#include <netinet/in.h>

namespace muduo
{
	class InetAddress : public muduo::copyable
	{
	public:
		explicit InetAddress(uint16_t port);
		InetAddress(const std::string& ip, uint16_t port);	
		

		void setSockAddrInet(const struct sockaddr_in& addr) { addr_ = addr; }
	
	private:
		struct sockaddr_in addr_;
	};
}

#endif