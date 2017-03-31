#pragma once
#include "concurrent_allocator.h"
#include "concurrent_queue.h"
#include "fiber_util.h"
#include "atomic_counter.h"

#define TASK_FUNCTION(name) void name(flib::fiber::fiber_scheduler* const sched, void* const data)
#define TASK_LAMBDA [](flib::fiber::fiber_scheduler* const sched, void* const data)

namespace flib
{
	namespace fiber
	{
		class fiber_scheduler;
		typedef void(*fiber_function)(fiber::fiber_scheduler* const, void* const data);
		
		enum fiber_priority {
			low,
			medium,
			high
		};

		struct task_decl
		{
			fiber_function func;
			fiber_priority prio;
			void* data;
		};

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
		};

		class fiber_scheduler
		{
		public:
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
		
	}
}