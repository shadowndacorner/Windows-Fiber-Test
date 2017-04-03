#pragma once
#include <stdint.h>
#include "atomic_counter.h"
#include "tagged_linear_allocator.h"

namespace flib
{
	class counter_allocator
	{
	public:
		counter_allocator(const uint64_t& key);
		atomic_counter* allocate();
	private:
		tagged_linear_allocator<atomic_counter> m_alloc;
		uint64_t m_key;
	};

	
}