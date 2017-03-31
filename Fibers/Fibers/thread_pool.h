#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "concurrent_queue.h"

namespace flib
{
	class thread_pool
	{
	public:
		thread_pool();
		thread_pool(const int& numThreads);
		~thread_pool();

	public:
		void post_job(const std::function<void()>&);
		int get_thread_count();

	private:
		friend void thread_func(flib::thread_pool*, const int threadid);
		void initialize(const int& threadCount);
		std::function<void()> wait_job();

		std::vector<std::thread> m_threads;
		std::atomic_ushort m_activeThreads;
		std::condition_variable m_cvar;
		std::mutex m_mutex;

		flib::concurrent_queue<std::function<void()>> tasks;
		//std::queue<std::function<void()>> tasks;
		bool m_running;
	};
}