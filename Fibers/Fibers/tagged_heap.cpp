#include "tagged_heap.h"
#define FLIB_HEAP_BLOCK_SIZE (2*1024*1024)
#define FLIB_MAX_HEAP_SIZE (1024*1024*1024)
#define FLIB_MAX_FREE_BLOCKS (FLIB_MAX_HEAP_SIZE / FLIB_HEAP_BLOCK_SIZE)

flib::tagged_heap::tagged_heap()
{
	
}

char* flib::tagged_heap::alloc_block(const uint64_t& key)
{
	char* ptr = NULL;
	std::unique_lock<std::mutex>(m_mutex);
	if (free_blocks.size() > 0)
	{
		ptr = free_blocks.front();
		free_blocks.pop();
	}
	else
	{
		ptr = new char[FLIB_HEAP_BLOCK_SIZE];
	}

	if (allocated_blocks.count(key) == 0)
		allocated_blocks.insert_or_assign(key, std::vector<char*>());

	allocated_blocks[key].push_back(ptr);
	return ptr;
}

void flib::tagged_heap::free(const uint64_t& key)
{
	std::unique_lock<std::mutex>(m_mutex);
	if (allocated_blocks.count(key) > 0)
	{
		std::vector<char*>& set = allocated_blocks.at(key);
		for (auto iter = set.begin(); iter != set.end(); ++iter)
		{
			if (free_blocks.size() * FLIB_HEAP_BLOCK_SIZE > FLIB_MAX_FREE_BLOCKS)
				free_blocks.push(*iter);
			else
				delete[] * iter;
		}

		set.clear();
		allocated_blocks.erase(key);
	}
}

flib::tagged_heap::~tagged_heap()
{
	for (auto iter = allocated_blocks.begin(); iter != allocated_blocks.end(); ++iter)
	{
		std::vector<char*>& set = (*iter).second;
		for (auto iter = set.begin(); iter != set.end(); ++iter)
		{
			free_blocks.push(*iter);
		}

		set.clear();
	}

	allocated_blocks.clear();
	while (free_blocks.size() > 0)
	{
		delete[] free_blocks.front();
		free_blocks.pop();
	}
}