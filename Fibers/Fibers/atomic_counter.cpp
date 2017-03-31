#include "atomic_counter.h"

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