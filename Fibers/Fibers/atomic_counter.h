#pragma once
#include <stdint.h>
#include <thread>
#include <atomic>
#include <list>
#include <allocators>
#include <mutex>

namespace flib
{
	class atomic_counter
	{
	public:
		atomic_counter();
		atomic_counter(std::allocator<atomic_counter>*);
		atomic_counter(const uint32_t&);
		atomic_counter(const atomic_counter&);

		void operator=(const uint32_t& t);
		atomic_counter& operator++();
		atomic_counter& operator--();

		void wait_for_value(const uint32_t& target);
		bool operator>(const uint32_t&);
		bool operator<(const uint32_t&);
		bool operator==(const uint32_t&);
		bool operator>=(const uint32_t&);
		bool operator<=(const uint32_t&);
	private:
		std::atomic_uint32_t m_cnt;
		struct wait_struct
		{
			void* task;
			uint32_t target;
		};

		//std::list<wait_struct> waiters;
		//std::mutex m_waitmut;

		std::atomic_uint16_t m_death_cnt;
		std::allocator<atomic_counter>* alloc;
		void resume_waiters();
	};
}

inline void flib::atomic_counter::operator=(const uint32_t & t)
{
	m_cnt = t;
}

inline flib::atomic_counter & flib::atomic_counter::operator++()
{
	++m_cnt;
	return *this;
}

inline flib::atomic_counter & flib::atomic_counter::operator--()
{
	--m_cnt;
	resume_waiters();
	return *this;
}

inline bool flib::atomic_counter::operator>(const uint32_t &v)
{
	return m_cnt > v;
}

inline bool flib::atomic_counter::operator<(const uint32_t &v)
{
	return m_cnt < v;
}

inline bool flib::atomic_counter::operator==(const uint32_t &v)
{
	return m_cnt == v;
}

inline bool flib::atomic_counter::operator>=(const uint32_t &v)
{
	return m_cnt >= v;
}

inline bool flib::atomic_counter::operator<=(const uint32_t &v)
{
	return m_cnt <= v;
}