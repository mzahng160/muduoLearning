#Makefile for async_log
CC = g++
FLAGS = -lpthread
DEBUG = -g

AsyncLogging_test : AsyncLogging_test.o AsyncLogging.o Logging.o LogStream.o ProcessInfo.o CountDownLatch.o LogFile.o FileUtil.o
	$(CC) -o AsyncLogging_test AsyncLogging_test.o AsyncLogging.o Logging.o LogStream.o ProcessInfo.o CountDownLatch.o LogFile.o\
	       FileUtil.o	$(FLAGS) $(DEBUG)
AsyncLogging_test.o :AsyncLogging_test.cc
	$(CC) -c AsyncLogging_test.cc $(DEBUG)
AsyncLogging.o : AsyncLogging.cc
	$(CC) -c AsyncLogging.cc $(DEBUG)
Logging.o : Logging.cc
	$(CC) -c Logging.cc $(DEBUG)
LogStream.o : LogStream.cc
	$(CC) -c LogStream.cc $(DEBUG)
ProcessInfo.o : ProcessInfo.cc
	$(CC) -c ProcessInfo.cc $(DEBUG)
CountDownLatch.o : ../CountDownLatch/CountDownLatch.cc
	$(CC) -c ../CountDownLatch/CountDownLatch.cc $(DEBUG)
LogFile.o : LogFile.cc
	$(CC) -c LogFile.cc $(DEBUG)
FileUtil.o : FileUtil.cc
	$(CC) -c FileUtil.cc $(DEBUG)
.PHONY:clean
clean :
	rm AsyncLogging_test AsyncLogging_test.o AsyncLogging.o Logging.o LogStream.o ProcessInfo.o CountDownLatch.o LogFile.o FileUtil.o
