#pragma once
#include <queue>
#include <mutex>

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

		T& front();
		T& back();
		T& pop_front();
		bool try_pop_front(T*);
		size_t size();
		void pop();
		void push(const T&);
	private:
		queue<T> m_queue;
		mutex m_mutex;
	};

	template<typename T>
	inline concurrent_queue<T>::concurrent_queue() : m_queue(){}

	template<typename T>
	inline concurrent_queue<T>::concurrent_queue(const concurrent_queue &other):m_queue(other.m_queue){}

	template<typename T>
	inline concurrent_queue<T>::~concurrent_queue(){}

	template<typename T>
	inline T& concurrent_queue<T>::front()
	{
		std::lock_guard<mutex> lock(m_mutex);
		return m_queue.front();
	}

	template<typename T>
	inline T & concurrent_queue<T>::back()
	{
		std::lock_guard<mutex> lock(m_mutex);
		return m_queue.back();
	}

	template<typename T>
	inline T& concurrent_queue<T>::pop_front()
	{
		std::lock_guard<mutex> lock(m_mutex);
		T& ret = m_queue.front();
		m_queue.pop();
		return ret;
	}

	template<typename T>
	inline bool concurrent_queue<T>::try_pop_front(T *tg)
	{
		std::lock_guard<mutex> lock(m_mutex);
		if (m_queue.size() == 0)
		{
			*tg = T();
			return false;
		}

		auto func = m_queue.front();
		*tg = func;
		m_queue.pop();
		return true;
	}

	template<typename T>
	inline size_t concurrent_queue<T>::size()
	{
		return m_queue.size();
	}

	template<typename T>
	inline void concurrent_queue<T>::pop()
	{
		std::lock_guard<mutex> lock(m_mutex);
		m_queue.pop();
	}

	template<typename T>
	inline void concurrent_queue<T>::push(const T &value)
	{
		std::lock_guard<mutex> lock(m_mutex);
		m_queue.push(value);
	}
}