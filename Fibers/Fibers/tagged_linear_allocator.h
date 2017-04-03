#pragma once
#include <assert.h>
#include "tagged_heap.h"
#include "thread_pool.h"
#include "microprofile/microprofile.h"
#include <cstdarg>
#include <unordered_map>
#include <new>

namespace flib
{
	template <typename T>
	class tagged_linear_allocator
	{
	public:
		tagged_linear_allocator(uint64_t key);
		~tagged_linear_allocator();
		T* allocate(const size_t& num);
		void free_all_blocks_with_key();
		void construct(T*, ...);
		void destroy(T*);
	private:
		struct block_data {
			char* start;
			char* end;
		};

		std::mutex m_mutex;
		std::unordered_map<int, block_data> thread_blocks;
	public:
		uint64_t key;
	};

	template<typename T>
	inline tagged_linear_allocator<T>::tagged_linear_allocator(uint64_t bkey)
	{
		key = bkey;
	}

	template<typename T>
	flib::tagged_linear_allocator<T>::~tagged_linear_allocator()
	{
		if (!thread_blocks.empty())
			thread_blocks.clear();
	}

	template<typename T>
	T* flib::tagged_linear_allocator<T>::allocate(const size_t& num)
	{
		MICROPROFILE_SCOPEI("memory", "tagged_linear_allocator::allocate", 0x803080);
		int id = flib::get_worker_id();
		if (thread_blocks.count(id) == 0)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			thread_blocks[id] = block_data();
			thread_blocks[id].start = nullptr;
			thread_blocks[id].end = nullptr;
		}

		char*& cur_ptr = thread_blocks[id].start;
		char*& block_end = thread_blocks[id].end;

		T* ptr;
		assert(sizeof(T) * num <= FLIB_HEAP_BLOCK_SIZE);
		if (((size_t)(block_end - cur_ptr)) < sizeof(T) * num)
		{
			//printf("Allocating new block...\n");
			cur_ptr = shared_heap().alloc_block(key);
			block_end = cur_ptr + FLIB_HEAP_BLOCK_SIZE;
		}

		ptr = reinterpret_cast<T*>(cur_ptr);
		cur_ptr += sizeof(T) * num;
		return ptr;
	}


	template<typename T>
	inline void tagged_linear_allocator<T>::free_all_blocks_with_key()
	{
		shared_heap.free(key);
		thread_blocks.clear();
	}
	template<typename T>
	inline void tagged_linear_allocator<T>::construct(T *ptr, ...)
	{
		new(ptr) T(...);
	}

	template<typename T>
	inline void tagged_linear_allocator<T>::destroy(T *ptr)
	{
		ptr->~T();
	}
}
