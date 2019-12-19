#include <vector>
#include <memory>
#include <mutex>
#include <stdio.h>
#include <assert.h>

class Foo
{
public:
	void doit() const;
};

typedef std::vector<Foo> FooList;
typedef std::shared_ptr<FooList> FooListPtr;
FooListPtr g_foos;
std::mutex mutex;

void post(const Foo& f)
{
	printf("post\n");
	if (!g_foos.unique())
	{
		g_foos.reset(new FooList(*g_foos));
		printf("copy the whole list\n");
	}
	assert(g_foos.unique());
	g_foos->push_back(f);
}

void traverse()
{
	FooListPtr foos;
	{
		std::lock_guard<std::mutex> lock(mutex);
		foos = g_foos;
		assert(!g_foos.unique());
	}

	for (std::vector<Foo>::const_iterator it = foos->begin();
		it != foos->end(); ++it)
	{
		it->doit();
	}
}

void Foo::doit() const
{
	Foo f;
	post(f);
}

int main()
{
	g_foos.reset(new FooList);
	Foo f;
	post(f);

	traverse();
}