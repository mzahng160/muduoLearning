LIB_SRC = 			\
	Acceptor.cc		\
	Buffer.cc		\
	Channel.cc		\
	Connector.cc		\
	EventLoop.cc		\
	EventLoopThread.cc	\
	EventLoopThreadPool.cc	\
	InetAddress.cc		\
	Poller.cc		\
	Socket.cc		\
	SocketsOps.cc		\
	TcpClient.cc		\
	TcpConnection.cc	\
	TcpServer.cc		\
	Timer.cc		\
	TimerQueue.cc

BINARIES = EchoServer EchoClient ChatServer ChatClient TestBuffer

all: $(BINARIES)

include ../reactor_lib.mk

EchoServer: EchoServer.cc
EchoClient: EchoClient.cc
ChatServer: ChatServer.cc
ChatClient: ChatClient.cc
TestBuffer: TestBuffer.cc