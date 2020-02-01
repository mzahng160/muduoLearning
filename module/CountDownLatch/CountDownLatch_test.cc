#include "CountDownLatch.h"
#include <thread>

using namespace LogModule;
CountDownLatch count_(5);

void fun()
{
	count_.wait();
	printf("[%d]", count_.getCount());
}

int main()
{
	std::thread th[5];
	for (int i = 0; i < 5; ++i)
	{
		th[i] = std::thread(fun);
		count_.countDown();
	}

	for (int i = 0; i < 5; ++i)
	{
		th[i].join();
	}
}