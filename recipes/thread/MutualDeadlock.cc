#include <mutex>
#include <set>
#include <thread>
#include <unistd.h>

class Request;
class Inventory 
{
public:
	void add(Request* req)
	{
		std::lock_guard<std::mutex> lock(InventoryMutex_);
		requests_.insert(req);
	}

	void remove(Request* req) __attribute__((noinline))
	{
		std::lock_guard<std::mutex> lock(InventoryMutex_);
		requests_.erase(req);
	}

	void printAll() const;

private:
	mutable std::mutex InventoryMutex_;
	std::set<Request*> requests_;
};

Inventory g_inventory;

class Request {
public:
	void process()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		g_inventory.add(this);
	}

	~Request() __attribute__((noinline))
	{
		std::lock_guard<std::mutex> lock(mutex_);
		sleep(1);
		g_inventory.remove(this);
	}

	void print() const __attribute__((noinline))
	{
		std::lock_guard<std::mutex> lock(mutex_);
	}
private:
	mutable std::mutex mutex_;
};

void Inventory::printAll() const
{
	std::lock_guard<std::mutex> lock(InventoryMutex_);
	sleep(1);
	for (Request* val : requests_)
		val->print();
	printf("Inventory::printAll() unlocked\n");
}

void threadFunc()
{
	Request * req = new Request;
	req->process();
	delete req;
}

int main()
{
	std::thread thread(threadFunc);
	usleep(500*1000);
	g_inventory.printAll();
	thread.join();
}