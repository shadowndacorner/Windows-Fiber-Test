#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>

namespace flib
{
	class thread_pool
	{
	public:
		thread_pool();
		thread_pool(const int& numThreads);
		~thread_pool();
	private:
		static friend void thread_func(flib::thread_pool*, const int threadid);
		void initialize(const int& threadCount);
		std::vector<std::thread> m_threads;
		
		std::atomic_ushort m_activeThreads;
		std::condition_variable m_cvar;
		bool m_running;
	};
}