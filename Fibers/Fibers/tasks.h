#pragma once
#include "atomic_counter.h"
#ifndef TASK_FUNCTION
#define TASK_FUNCTION(name) void name(flib::fiber::fiber_scheduler* const sched, void* const data)
#define TASK_LAMBDA [](flib::fiber::fiber_scheduler* const sched, void* const data)
#endif
namespace flib
{
	namespace fiber
	{
		class fiber_scheduler;
	}

	typedef void(*fiber_function)(fiber::fiber_scheduler* const, void* const data);

	enum fiber_priority {
		low,
		medium,
		high
	};

	struct task_decl
	{
		fiber_function func;
		fiber_priority prio = fiber_priority::low;
		void* data;
	};

	void RunTask(const task_decl& task, atomic_counter * const cnt);
	void RunTasks(task_decl* task, const size_t & size, atomic_counter * const cnt);
	fiber::fiber_scheduler* task_scheduler();

	namespace this_task
	{
		void yield();
	}
}