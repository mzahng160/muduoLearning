#include "TcpClient.h"

TcpClient(EventLoop* loop, const InetAddress& listenAddr);
~TcpClient();

void setConnectionCallback(const ConnectionCallback& cb)
{ connectionCallback_ = cb; }

void setMessageCallback(const MessageCallback& cb)
{ messageCallback_ = cb; }

void enableRetry();
void connect();