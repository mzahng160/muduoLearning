#include <mutex>
#include <set>
#include <thread>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

class Request;
class Inventory 
{
public:
	Inventory() :Requests(new requestList)
	{
	}

	void add(Request* req)
	{
		std::lock_guard<std::mutex> lock(InventoryMutex_);		
		if (!Requests.unique())
		{
			Requests.reset(new requestList(*Requests));
			printf("Inventory::add() copy the whole list\n");
		}
		assert(Requests.unique());
		Requests->insert(req);
	}

	void remove(Request* req) __attribute__((noinline))
	{
		std::lock_guard<std::mutex> lock(InventoryMutex_);
		if (!Requests.unique())
		{
			Requests.reset(new requestList(*Requests));
		}
		assert(Requests.unique());
		Requests->erase(req);
	}

	void printAll() const;

private:
	typedef std::set<Request*> requestList;
	typedef std::shared_ptr<requestList> requestListPtr;	

	requestListPtr getData() const
	{
		std::lock_guard<std::mutex> lock(InventoryMutex_);
		return Requests;
	}

	mutable std::mutex InventoryMutex_;
	requestListPtr Requests;	
};

Inventory g_inventory;

class Request {
public:

	Request() :x_(0) 
	{
	}

	~Request() __attribute__((noinline))
	{
		std::lock_guard<std::mutex> lock(mutex_);
		x_ = -1;
		sleep(1);
		g_inventory.remove(this);
	}

	void process()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		g_inventory.add(this);
	}

	void print() const __attribute__((noinline))
	{
		std::lock_guard<std::mutex> lock(mutex_);
		//....
		printf("print Request %p x=%d\n", this, x_);
	}
private:
	mutable std::mutex mutex_;
	int x_;
};

void Inventory::printAll() const
{
	requestListPtr tmpRequests;
	{
		std::lock_guard<std::mutex> lock(InventoryMutex_);
		tmpRequests = Requests;
		assert(!Requests.unique());
	}	

	sleep(1);

	for (std::set<Request*>::iterator it = tmpRequests->begin();
		it != tmpRequests->end(); ++it)
	{
		(*it)->print();
	}
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