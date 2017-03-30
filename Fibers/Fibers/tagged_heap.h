#pragma once
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <queue>
#include <mutex>

namespace flib
{
	class tagged_heap
	{
	public:
		tagged_heap();
		char* alloc_block(const uint64_t& key);
		void free(const uint64_t& key);
		~tagged_heap();
	private:
		std::unordered_map<uint64_t, std::vector<char*>> allocated_blocks;
		std::queue<char*> free_blocks;
		std::mutex m_alloc_lock;
	};

	static tagged_heap shared_heap;
}