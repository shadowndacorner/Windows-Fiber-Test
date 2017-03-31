#include "atomic_counter.h"
#include "fiber_scheduler.h"

flib::atomic_counter::atomic_counter()
{
	m_cnt = 0;
}

flib::atomic_counter::atomic_counter(const uint32_t & cnt)
{
	m_cnt = cnt;
}

flib::atomic_counter::atomic_counter(const atomic_counter &cnt)
{
	m_cnt = cnt.m_cnt.load();
}

void flib::atomic_counter::wait_for_value(uint32_t target)
{
	while (m_cnt != target)
	{
		if (flib::fiber_util::flIsFiber())
		{
			// TODO: optimize this to put it in a waiting queue
			flib::fiber::get_global_scheduler()->yield();
		}
		else
		{
			std::this_thread::yield();
		}
	}
}