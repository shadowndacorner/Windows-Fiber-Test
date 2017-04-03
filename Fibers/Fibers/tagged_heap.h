#pragma once
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <queue>
#include <mutex>

#define FLIB_HEAP_BLOCK_SIZE (2*1024*1024)
namespace flib
{
	class tagged_heap
	{
	public:
		tagged_heap();
		tagged_heap(size_t);
		char* alloc_block(const uint64_t& key);
		void free(const uint64_t& key);
		~tagged_heap();

	private:
		std::unordered_map<uint64_t, std::vector<char*>> allocated_blocks;
		std::queue<char*> free_blocks;
		std::mutex m_alloc_mutex;
	};

	tagged_heap& shared_heap();
}