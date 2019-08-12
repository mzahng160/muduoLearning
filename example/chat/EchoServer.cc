#include "EventLoop.h"
#include "InetAddress.h"
#include "TcpServer.h"
#include "logging/Logging.h"

#include <boost/bind.hpp>

using namespace muduo;

int numThreads = 0;

class EchoServer
{  
public:
  

  EchoServer(EventLoop* loop, const InetAddress& listenAddr)
      : loop_(loop),
        server_(loop, listenAddr)
  {
    server_.setConnectionCallback(
      boost::bind(&EchoServer::onConnection, this, _1));
    server_.setMessageCallback(
      boost::bind(&EchoServer::onMessage, this, _1, _2, _3));
    server_.setThreadNum(numThreads);
  }
  //~EchoServer(){}
  
  void start()
  {
    server_.start();
  }

  private:
    void onConnection(const TcpConnectionPtr& conn)
    {
      LOG_TRACE << conn->peerAddress().toHostPort() << " -> "
              << conn->localAddress().toHostPort() << " is "
              << (conn->connected() ? "Up" : "Down");

      //conn->send("hello\n");

      LOG_INFO << "onConnection: send hello";
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
    {

      std::string str(buf->peek(), buf->readableBytes());
      std::string msg(buf->retrieveAsString());
      LOG_TRACE << conn->name() << "recv " << msg.size() 
          << " bytes at " << time.toFormattedString();      

      if(msg == "exit\n")
      {
        conn->send("bye\n");
        conn->shutdown();
      }
      if(msg == "quit\n")
      {
        loop_->quit();
      }
      conn->send(msg);
    }

    EventLoop* loop_;
    TcpServer server_;
};

int main(int argc, char* argv[])
{
  LOG_INFO << "pid= " << getpid() << ", tid=" <<  CurrentThread::tid();  
  LOG_INFO << "sizeof TcpConnection = " << sizeof(TcpConnection);

  if(argc > 1)
  {
    numThreads = atoi(argv[1]);
  }

  EventLoop loop;
  InetAddress listenAddr(9981);
  EchoServer server(&loop, listenAddr);

  server.start();

  loop.loop();
}