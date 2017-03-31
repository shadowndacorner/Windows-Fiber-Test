#pragma once
#include "concurrent_allocator.h"
#include "concurrent_queue.h"
#include "fiber_util.h"
#include "atomic_counter.h"
#include "tasks.h"

namespace flib
{
	namespace fiber
	{
		class fiber_scheduler;
		struct fiber_data
		{
			fid_t fiber;
			fid_t yield_to;
			fiber_function func;
			flib::atomic_counter* counter;
			fiber_scheduler* sched;
			void* data;
			fiber_priority prio;
			bool isComplete;
			bool asleep;
		};

		class fiber_scheduler
		{
		public:
			void run_job(const task_decl& dat, flib::atomic_counter* const cnt);
			void run_jobs(task_decl* const dat, const size_t& count, flib::atomic_counter * const cnt = nullptr);
			void do_work();
			bool has_work();
			void yield();
		private:
			void schedule(fiber_data& dat);
			flib::concurrent_queue<fiber_data*> high_prio;
			flib::concurrent_queue<fiber_data*> med_prio;
			flib::concurrent_queue<fiber_data*> low_prio;
			flib::concurrent_allocator<fiber_data> data_alloc;

			static void fiber_func(void*);
		};

		fiber_scheduler* get_global_scheduler();
	}
}