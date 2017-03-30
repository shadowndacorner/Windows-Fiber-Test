#include "thread_ex.h"
#include "thread_pool.h"
#include <iostream>

flib::thread_pool::thread_pool()
{
	initialize(std::thread::hardware_concurrency());
}

flib::thread_pool::thread_pool(const int& numThreads)
{
	initialize(numThreads);
}

flib::thread_pool::~thread_pool()
{
	m_running = false;
	for (auto iter = m_threads.begin(); iter != m_threads.end(); ++iter)
	{
		(*iter).join();
	}
}

void flib::thread_pool::initialize(const int& num_threads)
{
	using namespace std;
	m_running = true;
	for (int i = 0; i < num_threads; ++i)
	{
		m_threads.push_back(thread(flib::thread_func, this, i));

		thread& thr = m_threads.back();
	}
	while (m_activeThreads < num_threads) { std::this_thread::yield(); }
}

void flib::thread_func(flib::thread_pool* m_pool, const int threadid)
{
	++m_pool->m_activeThreads;
	set_this_thread_affinity(threadid);
	// Wait until we're on the correct thread
	while (get_current_processor() != threadid) { std::this_thread::yield(); }
	while (m_pool->m_running)
	{
		// find a job
		// do a job
		// profit
	}
	--m_pool->m_activeThreads;
}
