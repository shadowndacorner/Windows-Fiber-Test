#pragma once
#include <assert.h>
#include "tagged_heap.h"

namespace flib
{
	template <typename T>
	class tagged_linear_allocator
	{
	public:
		tagged_linear_allocator(uint64_t key);
		T* allocate(const size_t& num = 1);
	private:
		uint64_t key;
		char* block_end;
		char* cur_ptr;
	};

	template<typename T>
	inline tagged_linear_allocator<T>::tagged_linear_allocator(uint64_t bkey)
	{
		key = bkey;
	}

	template<typename T>
	inline T* tagged_linear_allocator<T>::allocate(const size_t& num)
	{
		T* ptr;
		assert(sizeof(T) * num <= FLIB_HEAP_BLOCK_SIZE);
		if (((size_t)(block_end - cur_ptr)) < sizeof(T) * num)
		{
			//printf("Allocating new block...\n");
			cur_ptr = shared_heap.alloc_block(key);
			block_end = cur_ptr + FLIB_HEAP_BLOCK_SIZE;
		}

		ptr = reinterpret_cast<T*>(cur_ptr);
		cur_ptr += sizeof(T) * num;
		return ptr;
	}
}
