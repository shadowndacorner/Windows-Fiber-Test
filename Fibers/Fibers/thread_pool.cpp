#include "thread_ex.h"
#include "thread_pool.h"
#include <iostream>

void flib::thread_pool::post_job(const std::function<void()>& func)
{
	{
		std::unique_lock<std::mutex>(m_mutex);
		tasks.push(func);
	}
	m_cvar.notify_one();
}

int flib::thread_pool::get_thread_count()
{
	return m_threads.size();
}

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
		m_cvar.notify_all();
		(*iter).join();
		m_cvar.notify_all();
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

std::function<void()> flib::thread_pool::wait_job()
{
	std::function<void()> ret;
	std::unique_lock<std::mutex> lock(m_mutex);
	while (tasks.size() == 0 && m_running)
	{
		m_cvar.wait(lock);
	}

	if (!m_running)
		return 0;
	ret = tasks.front();
	tasks.pop();
	return ret;
}

void flib::thread_func(flib::thread_pool* m_pool, const int threadid)
{
	++m_pool->m_activeThreads;
	set_this_thread_affinity(threadid);

	// Wait until we're on the correct thread
	while (get_current_processor() != threadid) { std::this_thread::yield(); }
	while (m_pool->m_running)
	{
		auto job = m_pool->wait_job();
		if (job != 0)
			job();
	}
	--m_pool->m_activeThreads;
}
