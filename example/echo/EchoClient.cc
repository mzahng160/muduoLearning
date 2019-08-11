#include "TcpClient.h"

#include "InetAddress.h"
#include "EventLoop.h"
#include "logging/Logging.h"

#include <boost/noncopyable.hpp>
//#include <>

using namespace muduo;
using namespace std::placeholders;

int current = 0;
class EchoClient;
std::vector<std::unique_ptr<EchoClient>> clients;

class EchoClient : boost::noncopyable
{
public:
  EchoClient(EventLoop* loop, const InetAddress& listenAddr, int index)
      : loop_(loop),
        client_(loop, listenAddr),
        index_(index)
  {
    client_.setConnectionCallback(
      std::bind(&EchoClient::onConnection, this, _1));
    client_.setMessageCallback(
      std::bind(&EchoClient::onMessage, this, _1, _2, _3));
  }
  //~EchoClient();
  
  void connect()
  {
    client_.connect();
  }

  private:
    void onConnection(const TcpConnectionPtr& conn)
    {
#if 1     
      LOG_TRACE << conn->localAddress().toHostPort() 
          << " -> " << conn->peerAddress().toHostPort()
          << " is " << (conn->connected() ? "Up" : "Down");
#endif
      if(conn->connected())
      {
        ++current;
        if(implicit_cast<size_t>(current) < clients.size())
        {
          clients[current]->connect();  
        }
        LOG_INFO << "*** connected " << current;        
      }

      char x = (char)(static_cast<int>('A') + index_);
      std::string str(10, x);
      conn->send(str);
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
    {

      std::string str(buf->peek(), buf->readableBytes());
      std::string msg(buf->retrieveAsString());         
      LOG_TRACE << conn->name() << " recv " << msg.size()
            << " bytes at " << time.toFormattedString();

      if(str != msg)      
        LOG_INFO << "##### client-"<< index_ << " message:[" << msg <<"] receive buf:[" << str << "]";
 
      if(msg == "quit\n")
      {
        conn->send("bye\n");
        conn->shutdown();
      }
      else if(msg == "shutdown\n")
      {
        loop_->quit();
      }
      else
      {
        conn->send(msg);
        //conn->shutdown();
      }
    }

    EventLoop* loop_;
    TcpClient client_;
    int index_;
};

int main(int argc, char* argv[])
{
  LOG_INFO << "pid= " << getpid() << ", tid=" << CurrentThread::tid();
  //printf("pid=%d tid=%d\n", getpid(), CurrentThread::tid());

  
  EventLoop loop;  						  
  InetAddress serverAddr("localhost",9981);

  int n = 1;
  if(argc > 1)
  {
    n = atoi(argv[1]);
  }

  clients.reserve(n);

  for(int i = 0; i < n; ++i)
  {
    clients.emplace_back(new EchoClient(&loop, serverAddr, i));
  }

  clients[current]->connect();
  loop.loop();
}
