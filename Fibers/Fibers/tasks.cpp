#include "tasks.h"
#include "fiber_scheduler.h"
#include <assert.h>

namespace flib
{
	using namespace flib::fiber;
	using namespace flib::fiber_util;

	void RunTask(const task_decl& task, atomic_counter * const cnt)
	{
		get_global_scheduler()->run_job(task, cnt);
	}

	void RunTasks(task_decl* task, const size_t & size, atomic_counter * const cnt)
	{
		get_global_scheduler()->run_jobs(task, size, cnt);
	}

	void WaitForCounter(atomic_counter * const cnt, const uint32_t& tg)
	{
		cnt->wait_for_value(tg);
	}

	void flib::this_task::yield()
	{
		assert(flIsFiber());
		get_global_scheduler()->yield();
	}

	fiber::fiber_scheduler* task_scheduling::task_scheduler()
	{
		return flib::get_global_scheduler();
	}
}

void flib::task_scheduling::execute_tasks()
{
	flib::get_global_scheduler()->do_work();
}
