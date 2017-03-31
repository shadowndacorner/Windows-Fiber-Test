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

	typedef void(*task_function)(fiber::fiber_scheduler* const, void* const data);

	enum task_priority {
		low,
		medium,
		high
	};

	struct task_decl
	{
		task_function func;
		task_priority prio = task_priority::low;
		void* data;
	};

	void RunTask(const task_decl& task, atomic_counter * const cnt);
	void RunTasks(task_decl* task, const size_t & size, atomic_counter * const cnt);
	void WaitForCounter(atomic_counter* const cnt, const uint32_t& tg);

	namespace task_scheduling
	{
		fiber::fiber_scheduler* task_scheduler();
		void execute_tasks();
	}

	namespace this_task
	{
		void yield();
	}
}