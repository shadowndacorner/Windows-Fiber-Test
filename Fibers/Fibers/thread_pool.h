#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include "atomic_counter.h"
#include <condition_variable>
#include "concurrent_queue.h"

namespace flib
{
	typedef void (*tp_task_func)(void* const);

	struct task_struct
	{
		task_struct() :task(NULL), data(NULL) {};
		task_struct(tp_task_func t, void* dat) : task(t), data(dat) {};
		tp_task_func task;
		void* data;
	};

	namespace thread_pool_internal
	{
		struct ctr_task_start_data
		{
			task_struct task;
			std::allocator<ctr_task_start_data>* alloc;
			std::mutex* mut;
			flib::atomic_counter* ctr;
		};
	}

	int get_worker_id();

	class thread_pool
	{
	public:
		thread_pool();
		thread_pool(const int& numThreads);
		~thread_pool();

	public:
		void post_job(const tp_task_func&, void* const data, flib::atomic_counter* cnt);
		int get_thread_count();
		bool do_work_if_available();

	private:
		friend void thread_func(flib::thread_pool*, const int threadid);
		void initialize(const int& threadCount);
		task_struct wait_job();

		std::vector<std::thread> m_threads;
		std::atomic_ushort m_activeThreads;
		std::condition_variable m_cvar;
		std::mutex m_mutex;

		flib::concurrent_queue<task_struct> tasks;
		std::allocator<thread_pool_internal::ctr_task_start_data> m_alloc;

		bool m_running;
	};
}