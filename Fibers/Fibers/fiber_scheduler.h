#pragma once
#include "concurrent_allocator.h"
#include "concurrent_queue.h"
#include "fiber_util.h"
#include "atomic_counter.h"
#include "tasks.h"
#include "stdint.h"

namespace flib
{
	namespace fiber
	{
		class fiber_scheduler;
		struct fiber_data
		{
			fid_t fiber;
			fid_t yield_to;
			task_function func;
			flib::atomic_counter* counter;
			fiber_scheduler* sched;
			void* data;
			uint32_t frame_number;
			task_priority prio;
			bool isComplete;
			bool asleep;
		};

		class fiber_scheduler
		{
		public:
			void run_job(const task_decl& dat, flib::atomic_counter** const cnt);
			void run_jobs(task_decl* const dat, const size_t& count, flib::atomic_counter ** const cnt);
			void do_work();
			bool has_work();
			void yield();
		private:
			friend class flib::atomic_counter;

			void schedule(fiber_data& dat);
			void batch_schedule(fiber_data* fdat, const size_t& num, const uint32_t& mask);
			flib::concurrent_queue<fiber_data*> high_prio;
			flib::concurrent_queue<fiber_data*> med_prio;
			flib::concurrent_queue<fiber_data*> low_prio;
			flib::concurrent_allocator<fiber_data> data_alloc;
			//std::allocator<atomic_counter> counter_alloc;

			static void fiber_func(void*);
		};

		fiber_scheduler* get_global_scheduler();
		fiber_data* get_active_data();
	}
}