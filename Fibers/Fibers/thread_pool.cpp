#include "thread_ex.h"
#include "thread_pool.h"
#include <iostream>
#include "microprofile/microprofile.h"

void flib::thread_pool::post_job(const tp_task_func& func, void* const data, flib::atomic_counter* cnt)
{
	task_struct task;
	if (cnt != NULL)
	{
		++(*cnt);

		using namespace flib::thread_pool_internal;
		ctr_task_start_data* m_dat;
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_dat = m_alloc.allocate(1);
			m_dat->alloc = &m_alloc;
			m_dat->ctr = cnt;
			m_dat->task.data = data;
			m_dat->task.task = func;
			m_dat->mut = &m_mutex;
		}

		tasks.push(task_struct([](void* const data) {
			auto ptr = reinterpret_cast<ctr_task_start_data*>(data);
			ctr_task_start_data dat = *ptr;
			
			dat.task.task(dat.task.data);
			--(*dat.ctr);
			std::unique_lock<std::mutex> lock(*(dat.mut));
			dat.alloc->deallocate(ptr, 1);
		}, reinterpret_cast<void*>(m_dat)));
	}
	else
	{
		tasks.push(task_struct(func, data));
	}
	m_cvar.notify_one();
}

namespace flib
{
	static thread_local int threadID=-1;
	int get_worker_id()
	{
		return threadID;
	}
	void thread_func(flib::thread_pool* m_pool, const int threadid)
	{
		MicroProfileOnThreadCreate("worker_thread");
		threadID = threadid;
		++m_pool->m_activeThreads;
		set_this_thread_affinity(threadid);

		// Wait until we're on the correct thread
		while (get_current_processor() != threadid) { std::this_thread::yield(); }
		while (m_pool->m_running)
		{
			auto job = m_pool->wait_job();
			if (job.task != 0)
				job.task(job.data);
		}
		--m_pool->m_activeThreads;
		MicroProfileOnThreadExit();
	}
}
int flib::thread_pool::get_thread_count()
{
	return (int)m_threads.size();
}

bool flib::thread_pool::do_work_if_available()
{
	task_struct task;
	if (tasks.try_pop_front(task))
	{
		task.task(task.data);
		return true;
	}
	return false;
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

flib::task_struct flib::thread_pool::wait_job()
{
	task_struct ret;
	std::unique_lock<std::mutex> lock(m_mutex);
	while (tasks.empty())
	{
		if (!m_running)
			return ret;
		m_cvar.wait(lock);
	}

	tasks.try_pop_front(ret);
	return ret;
}
