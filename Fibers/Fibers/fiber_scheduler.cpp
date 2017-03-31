#include "fiber_scheduler.h"

using namespace flib::fiber;
using namespace flib::fiber_util;

static thread_local fid_t current_main;
static thread_local fiber_data* active_fiber;
static flib::fiber::fiber_scheduler global_sched;
void flib::fiber::fiber_scheduler::run_job(const task_decl & dat, flib::atomic_counter * const cnt)
{
	fiber_data* data = data_alloc.allocate(1);
	fiber_data& fdat = data[0];

	if (cnt != nullptr)
		++(*cnt);
	fdat.func = dat.func;
	fdat.prio = dat.prio;
	fdat.data = dat.data;
	fdat.isComplete = false;
	fdat.asleep = false;
	fdat.sched = this;
	fdat.counter = cnt;
	fdat.fiber = flCreateFiber(128 * 1024, fiber_func, &fdat);
	schedule(fdat);
}

void flib::fiber::fiber_scheduler::run_jobs(task_decl* const dat, const size_t& count, flib::atomic_counter * const cnt)
{
	// TODO: optimize this
	fiber_data* data = data_alloc.allocate(count);
	for (size_t i = 0; i < count; ++i)
	{
		fiber_data& fdat = data[i];
		if (cnt != nullptr)
			++(*cnt);
		fdat.func = dat[i].func;
		fdat.prio = dat[i].prio;
		fdat.data = dat[i].data;
		fdat.isComplete = false;
		fdat.asleep = false;
		fdat.sched = this;
		fdat.counter = cnt;
		fdat.fiber = flCreateFiber(128 * 1024, fiber_func, &fdat);
		schedule(fdat);
	}
}

void flib::fiber::fiber_scheduler::do_work()
{
	using namespace flib::fiber_util;
	current_main = flInitializeThread();
	fiber_data* target;

	// Try to pop from high priority, then medium, then low
	while (high_prio.try_pop_front(&target) || med_prio.try_pop_front(&target) || low_prio.try_pop_front(&target))
	{
		// If we can, then switch to the new fiber
		target->yield_to = flGetCurrentFiber();
		active_fiber = target;
		flFiberSwitch(target->fiber);
		if (!target->isComplete)
		{
			schedule(*target);
		}
		else
		{
			if (target->counter != nullptr)
				--(*target->counter);

			flDeleteFiber(target->fiber);
		}
	}

	flDeinitializeThread();
}

bool flib::fiber::fiber_scheduler::has_work()
{
	return high_prio.size() == 0 && med_prio.size() == 0 && low_prio.size() == 0;
}

void flib::fiber::fiber_scheduler::yield()
{
	flFiberSwitch(current_main);
}

void flib::fiber::fiber_scheduler::schedule(fiber_data & fdat)
{
	switch (fdat.prio)
	{
	case low:
		low_prio.push(&fdat);
		break;
	case medium:
		med_prio.push(&fdat);
		break;
	case high:
		high_prio.push(&fdat);
		break;
	}
}

void flib::fiber::fiber_scheduler::fiber_func(void* data)
{
	fiber_data* dat = reinterpret_cast<fiber_data*>(data);
	dat->func(dat->sched, dat->data);
	dat->isComplete = true;
	dat->sched->yield();
}

flib::fiber::fiber_scheduler * flib::fiber::get_global_scheduler()
{
	return &global_sched;
}
