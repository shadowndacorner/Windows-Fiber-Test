#include <algorithm>
#include "fiber_scheduler.h"
#include "fiber_util.h"
#include "frame_data.h"
#include "ska_sort.h"
#include "microprofile/microprofile.h"

using namespace flib::fiber;
using namespace flib::fiber_util;


static thread_local fid_t current_main;
static thread_local fiber_data* active_fiber;
static flib::fiber::fiber_scheduler global_sched;
void flib::fiber::fiber_scheduler::run_job(const task_decl & dat, flib::atomic_counter ** const cnt)
{	
	if (cnt != nullptr)
	{
		if (*cnt == nullptr)
		{
			auto frame = GetActiveFrameData();
			*cnt = frame->allocate<flib::atomic_counter>(1);
			frame->construct<flib::atomic_counter>(*cnt, nullptr);
			//*cnt = counter_alloc.allocate(1);
			//counter_alloc.construct(*cnt, &counter_alloc);
		}
	}
	fiber_data* data = data_alloc.allocate(1);
	fiber_data& fdat = data[0];

	if (cnt != nullptr && *cnt != nullptr)
	{
		++(**cnt);
		fdat.counter = *cnt;
	}
	else
		fdat.counter = nullptr;

	fdat.frame_number = GetActiveFrameData()->m_frameNum;
	fdat.func = dat.func;
	fdat.prio = dat.prio;
	fdat.data = dat.data;
	fdat.isComplete = false;
	fdat.asleep = false;
	fdat.sched = this;
	fdat.fiber = flCreateFiber(128 * 1024, fiber_func, &fdat);
	schedule(fdat);
}

void flib::fiber::fiber_scheduler::run_jobs(task_decl* const dat, const size_t& count, flib::atomic_counter ** const cnt)
{
	// TODO: optimize this, especially the queue accesses
	if (cnt != nullptr && *cnt == nullptr)
	{
		auto frame = GetActiveFrameData();
		*cnt = frame->allocate<flib::atomic_counter>(1);
		frame->construct<flib::atomic_counter>(*cnt, nullptr);
	}

	uint32_t mask = 0;
	fiber_data* data = data_alloc.allocate(count);
	for (size_t i = 0; i < count; ++i)
	{
		fiber_data& fdat = data[i];
		if (*cnt != nullptr)
			++(**cnt);
		fdat.func = dat[i].func;

		if (dat[i].prio != low && dat[i].prio != medium && dat[i].prio != high)
			dat[i].prio = medium;

		fdat.prio = dat[i].prio;
		mask |= fdat.prio;

		fdat.data = dat[i].data;
		fdat.isComplete = false;
		fdat.asleep = false;
		fdat.sched = this;
		fdat.counter = *cnt;

		// TODO: cache fibers
		fdat.fiber = flCreateFiber(128 * 1024, fiber_func, &fdat);
	}

	batch_schedule(data, count, mask);
}

void flib::fiber::fiber_scheduler::do_work()
{
	using namespace flib::fiber_util;
	current_main = flInitializeThread();
	fiber_data* target;

	// Try to pop from high priority, then medium, then low
	while (high_prio.try_pop_front(target) || med_prio.try_pop_front(target) || low_prio.try_pop_front(target))
	{
		target->yield_to = flGetCurrentFiber();
		active_fiber = target;
		{
			flFiberSwitch(target->fiber);
		}
		active_fiber = nullptr;
		if (!target->isComplete)
		{
			schedule(*target);
		}
		else
		{
			if (target->counter != nullptr)
			{
				--(*target->counter);
				target->counter = nullptr;
			}
			flDeleteFiber(target->fiber);
			data_alloc.deallocate(target, 1);
		}
	}

	flDeinitializeThread();
}

bool flib::fiber::fiber_scheduler::has_work()
{
	return !high_prio.empty() && med_prio.empty() && low_prio.empty();//high_prio.size() == 0 && med_prio.size() == 0 && low_prio.size() == 0;
}

void flib::fiber::fiber_scheduler::yield()
{
	flFiberSwitch(current_main);
}

void flib::fiber::fiber_scheduler::schedule(fiber_data & fdat)
{
	// If we're sleeping, allow someone else to hold us
	if (fdat.asleep)
		return;

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
	default:
		med_prio.push(&fdat);
		break;
	}
}

void flib::fiber::fiber_scheduler::batch_schedule(fiber_data* fdata, const size_t& num, const uint32_t& mask)
{
	for (size_t i = 0; i < num; ++i)
	{
		fiber_data& fdat = fdata[i];
		
		// If we're sleeping, allow someone else to hold us
		if (fdat.asleep)
			return;

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
		default:
			med_prio.push(&fdat);
			break;
		}
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

fiber_data * flib::fiber::get_active_data()
{
	return active_fiber;
}
