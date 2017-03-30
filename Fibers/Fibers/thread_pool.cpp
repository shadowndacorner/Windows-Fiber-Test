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
	for (unsigned int i = 0; i < num_threads; ++i)
	{
		auto func = [this, i] {
			++m_activeThreads;
			while (get_current_processor() != i) { std::this_thread::yield(); }

			std::cout << "Thread " << i << " initialized\n";
			while (m_running)
			{
				// do work
				auto aff = get_current_processor();
				if (aff != i)
				{
					std::cout << "ERROR!  Thread " << i << " running on processor " << aff << "!\n";
				}
				std::this_thread::yield();
			}
			std::cout << "Thread " << i << " finished\n";
		};

		m_threads.push_back(thread(func));

		thread& thr = m_threads.back();
		set_thread_affinity(thr, i);
	}

	while (num_threads < m_activeThreads) { std::this_thread::yield(); }
}
