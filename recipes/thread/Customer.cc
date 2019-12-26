#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <assert.h>

class CustomerData
{
public:
	CustomerData() : data_(new Map)
	{
	}
	~CustomerData() {}

	int query(std::string& customer, const std::string& stock) const;

private:
	typedef std::pair<std::string, int> Entry;
	typedef std::vector<Entry> EntryList;
	typedef std::map<std::string, EntryList> Map;
	typedef std::shared_ptr<Map> MapPtr;

	void update(const std::string& constumer, const EntryList& entries);
	void update(const std::string& message);
	
	static int findEntry(const EntryList& entries, const std::string& stock);
	static MapPtr parseData(const std::string& message);

	MapPtr getData()const
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return data_;
	}


	mutable std::mutex mutex_;
	MapPtr data_;
};

int CustomerData::query(std::string& customer, const std::string& stock) const
{
	MapPtr data = getData();

	Map::const_iterator entries = data->find(customer);
	if (entries != data->end())
		return findEntry(entries->second, stock);
	else
		-1;
}

void CustomerData::update(const std::string& constumer, const EntryList& entries)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (!data_.unique())
	{
		MapPtr newData(new Map(*data_));
		data_.swap(newData);
	}

	assert(data_.unique());
	(*data_)[constumer] = entries;

}

void CustomerData::update(const std::string& message)
{	
}

int CustomerData::findEntry(const EntryList& entries, const std::string& stock)
{
	//std::lower_bound(entries.begin(), entries.end(), [](Entry tmp) {return 0 == tmp.first.compare(stock)});
}

int main()
{
	CustomerData data;
}