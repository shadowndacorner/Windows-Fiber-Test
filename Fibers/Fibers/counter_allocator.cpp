#include "counter_allocator.h"

flib::counter_allocator::counter_allocator(const uint64_t& key)
	:m_key(key), m_alloc(key)
{
}

flib::atomic_counter * flib::counter_allocator::allocate()
{
	return m_alloc.allocate(1);
}
