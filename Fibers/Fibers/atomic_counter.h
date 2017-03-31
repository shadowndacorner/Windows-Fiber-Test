#pragma once
#include <stdint.h>
#include <atomic>

namespace flib
{
	/*
	class Counter
	{
	public:
		Counter(const uint32_t&);
		Counter(const Counter&);
		Counter& operator =(const uint32_t&);
		Counter& operator ++();
		Counter operator ++(int);
	private:
		std::atomic_uint32_t m_cnt;
	};*/
	typedef std::atomic_uint16_t atomic_counter;
}