#include "EventLoop.h"
#include <iostream>

muduo::EventLoop* g_loop;

void print(){ std::cout << "time out" << std::endl;}


void thredFunc()
{
	g_loop->runAfter(1.0, print);
}

int main()
{
	muduo::EventLoop loop;
	g_loop = &loop;

	//loop.runAfter(2, print);
	muduo::Thread t(thredFunc);

	t.start();
	loop.loop();
}
