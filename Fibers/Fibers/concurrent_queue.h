#pragma once
//#include <queue>
#include <mutex>
#include <atomic>

//#define CQ_LOCKLESS
//#define CQ_LOCKLESS
#ifdef CQ_LOCKLESS
//#include "camel_concurrent.h"
#include <boost/lockfree/queue.hpp>
#else
#include <queue>
#endif

namespace flib
{
	using namespace std;
	template <typename T>
	class concurrent_queue
	{
	public:
		concurrent_queue();
		concurrent_queue(const concurrent_queue&);
		~concurrent_queue();

		bool try_pop_front(T&);
		//size_t size();
		bool empty();
		void push(const T&);
		//T& front();
		//T& back();
		//T& pop_front();
		//void pop();
		/*void unsafe_push(const T&);
		void unsafe_lock();
		void unsafe_unlock();*/
	private:
		/*vector<T> m_data;
		atomic_int start;
		atomic_int end;*/

#ifdef CQ_LOCKLESS
		boost::lockfree::queue<T> m_queue;
		//moodycamel::ConcurrentQueue<T> m_queue;
#else
		queue<T> m_queue;
		mutex m_mutex;
#endif
	};

	template<typename T>
	inline concurrent_queue<T>::concurrent_queue() :
#ifdef CQ_LOCKLESS
		m_queue(512)
#else
		m_queue()
#endif
	{}

	template<typename T>
	inline concurrent_queue<T>::concurrent_queue(const concurrent_queue &other) : m_queue(other.m_queue) {}

	template<typename T>
	inline concurrent_queue<T>::~concurrent_queue(){}
	/*
	template<typename T>
	inline T& concurrent_queue<T>::front()
	{
		//std::lock_guard<mutex> lock(m_mutex);
		
	}

	template<typename T>
	inline T & concurrent_queue<T>::back()
	{
		//std::lock_guard<mutex> lock(m_mutex);
		return m_data[end];
	}
	
	template<typename T>
	inline T& concurrent_queue<T>::pop_front()
	{
		
		//std::lock_guard<mutex> lock(m_mutex);
		return m_data[start--];
	}*/

	template<typename T>
	inline bool concurrent_queue<T>::try_pop_front(T& tg)
	{
#ifdef CQ_LOCKLESS
		return m_queue.pop(tg);
#else
		lock_guard<mutex> lock(m_mutex);
		if (m_queue.size() == 0)
		{
			tg = T();
			return false;
		}

		auto func = m_queue.front();
		tg = func;
		m_queue.pop();
		return true;
#endif
	}
	/*
	template<typename T>
	inline size_t concurrent_queue<T>::size()
	{
#ifdef CQ_LOCKLESS
		return (size_t)!m_queue.empty();
#else
		return m_queue.size();
#endif
	}*/

	template<typename T>
	inline bool concurrent_queue<T>::empty()
	{
		return m_queue.empty();
	}
	
	/*template<typename T>
	inline void concurrent_queue<T>::pop()
	{

	}*/

	template<typename T>
	inline void concurrent_queue<T>::push(const T &value)
	{
		/*
		int e = end++;
		if (m_data.size() > size())
			m_data.push_back(value);
		else
			m_data[e] = value;*/
#ifdef CQ_LOCKLESS
		assert(m_queue.push(value));
#else
		lock_guard<mutex> lock(m_mutex);
		m_queue.push(value);
#endif
	}

	/*
	template<typename T>
	inline void concurrent_queue<T>::unsafe_push(T &&val)
	{
		push(val);
	}
	
	template<typename T>
	inline void concurrent_queue<T>::unsafe_lock()
	{
		//m_mutex.lock();
	}
	
	template<typename T>
	inline void concurrent_queue<T>::unsafe_unlock()
	{
		//m_mutex.unlock();
	}*/
}