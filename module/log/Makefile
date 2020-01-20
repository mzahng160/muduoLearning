CC = g++ 
CCFLAGS = -g

#$(TARGET): Logging.cc  Logging_test.cc LogStream.cc ../ThreadPool/ThreadPool.cc LogFile.cc FileUtil.cc ProcessInfo.cc
#	$(CC) -o $(TARGET) Logging.cc Logging_test.cc LogStream.cc ../ThreadPool/ThreadPool.cc LogFile.cc FileUtil.cc ProcessInfo.cc  -lpthread $(CCFLAGS)


all:Logging_test
.PHONY : all
Logging_test :  Logging.cc  Logging_test.cc LogStream.cc ../ThreadPool/ThreadPool.cc LogFile.cc FileUtil.cc ProcessInfo.cc
	$(CC) -o Logging_test  Logging.cc Logging_test.cc LogStream.cc ../ThreadPool/ThreadPool.cc LogFile.cc FileUtil.cc ProcessInfo.cc  -lpthread $(CCFLAGS)

#clean: 
#	rm $(TARGET)
