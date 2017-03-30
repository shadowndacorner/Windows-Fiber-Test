#pragma once
#include <vector>
#include <thread>
#ifdef _WIN32
#include <atomic>
#else
#include <cstdatomic>
#endif

namespace flib
{
	class thread_pool
	{
	public:
		thread_pool();
		thread_pool(const int& numThreads);
		~thread_pool();
	private:
		void initialize(const int& threadCount);
		std::vector<std::thread> m_threads;
		std::atomic_ushort m_activeThreads;
		bool m_running;
	};
}