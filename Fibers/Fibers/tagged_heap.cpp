#include "tagged_heap.h"
#define FLIB_MAX_HEAP_SIZE (1024*1024*1024)
#define FLIB_MAX_FREE_BLOCKS (FLIB_MAX_HEAP_SIZE / FLIB_HEAP_BLOCK_SIZE)

flib::tagged_heap::tagged_heap()
{
	
}

flib::tagged_heap::tagged_heap(size_t numFreeBlocks)
{
	std::lock_guard<std::mutex> lock(m_alloc_mutex);
	for (size_t i = 0; i < numFreeBlocks; ++i)
	{
		free_blocks.push(new char[FLIB_HEAP_BLOCK_SIZE]);
	}
}

char* flib::tagged_heap::alloc_block(const uint64_t& key)
{
	std::lock_guard<std::mutex> lock(m_alloc_mutex);
	char* ptr = NULL;
	if (free_blocks.size() > 0)
	{
		ptr = free_blocks.front();
		free_blocks.pop();
	}
	else
	{
		ptr = new char[FLIB_HEAP_BLOCK_SIZE];
		printf("\tNew block\n");
	}

	if (allocated_blocks.count(key) == 0)
		allocated_blocks[key] = std::vector<char*>();

	allocated_blocks[key].push_back(ptr);
	return ptr;
}

void flib::tagged_heap::free(const uint64_t& key)
{
	std::lock_guard<std::mutex> lock(m_alloc_mutex);
	if (allocated_blocks.count(key) > 0)
	{
		std::vector<char*>& set = allocated_blocks.at(key);
		printf("\tFreeing %zd blocks\n", set.size());
		for (auto iter = set.begin(); iter != set.end(); ++iter)
		{
			if (free_blocks.size() < FLIB_MAX_FREE_BLOCKS)
				free_blocks.push(*iter);
			else
			{
				printf("\t\tKilling block\n");
				delete[] * iter;
			}
		}

		set.clear();
		allocated_blocks.erase(key);
	}
}

flib::tagged_heap::~tagged_heap()
{
	std::lock_guard<std::mutex> lock(m_alloc_mutex);
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

namespace flib
{
	static tagged_heap m_shared_heap;
	tagged_heap & flib::shared_heap()
	{
		return m_shared_heap;
	}
}
