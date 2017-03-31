#pragma once
// TODO: UNFINISHED
#include <list>
#include <mutex>
namespace flib
{
	template <typename T>
	class concurrent_list
	{
	public:
		concurrent_list()=default;
		concurrent_list(const std::list<T>& l) : m_list(l){}
		
		auto begin()
		{
			return m_list.begin();
		}
		
		auto end()
		{
			return m_list.end();
		}

		auto rbegin()
		{
			return m_list.rbegin();
		}

		auto rend()
		{
			return m_list.rend();
		}

		bool empty()
		{
			return m_list.empty();
		}

		size_t size()
		{
			return m_list.size();
		}

		size_t max_size()
		{
			return m_list.max_size();
		}

		T& front()
		{
			return m_list.front();
		}

		T& back()
		{
			return m_list.back();
		}


	private:
		std::list<T> m_list;
		std::mutex m_mutex;
	};
}