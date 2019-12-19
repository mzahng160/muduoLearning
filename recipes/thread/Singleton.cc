#include <pthread.h>
#include <vector>
#include <assert.h>
#include <iostream>

namespace detail
{
	template<typename T>
	struct has_no_destory
	{
		template <typename C> static char test(decltype(&C::no_destory));
		template <typename C> static __int32_t test(...);
		const static bool value = sizeof(test<T>(0)) == 1;
	};
}

template <typename T>
class Singleton
{
public:
	Singleton() = delete;
	~Singleton() = delete;

	static T& instance()
	{
		pthread_once(&ponce_, &Singleton::init);
		assert(nullptr != value_);
		return *value_;
	}

private:
	static void init()
	{
		value_ = new T();
	}

private:
	static pthread_once_t ponce_;
	static T* value_;
};

template<typename T>
T* Singleton<T>::value_ = nullptr;

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

int main()
{
	std::vector<int>& foo = Singleton<std::vector<int>>::instance();
	foo.emplace_back(1);
	foo.emplace_back(2);

	std::cout << foo.size() << std::endl;
}