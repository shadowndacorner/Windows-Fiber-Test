#pragma once
#include <allocators>
#include <mutex>

namespace flib
{
	template <typename T>	
	class concurrent_allocator
	{
	public:
		T* allocate(const size_t& cnt);
		void deallocate(T* ptr, const size_t& cnt);
	private:
		std::allocator<T> m_alloc;
		std::mutex m_mutex;
	};

	template<typename T>
	inline T * concurrent_allocator<T>::allocate(const size_t & cnt)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_alloc.allocate(cnt);
	}

	template<typename T>
	inline void concurrent_allocator<T>::deallocate(T * ptr, const size_t & cnt)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_alloc.deallocate(ptr, cnt);
	}
}