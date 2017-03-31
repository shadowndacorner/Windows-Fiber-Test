#include "atomic_counter.h"
#include "fiber_scheduler.h"

flib::atomic_counter::atomic_counter()
{
	m_cnt = 0;
}

flib::atomic_counter::atomic_counter(const uint32_t & cnt)
{
	m_cnt = cnt;
}

flib::atomic_counter::atomic_counter(const atomic_counter &cnt)
{
	m_cnt = cnt.m_cnt.load();
}

void flib::atomic_counter::resume_waiters()
{
	std::unique_lock<std::mutex> lock(m_waitmut);
	for (auto iter = waiters.begin(); iter != waiters.end(); ++iter)
	{
		if ((*iter).target > m_cnt)
			return;

		auto task = reinterpret_cast<fiber::fiber_data*>((*iter).task);
		task->asleep = false;
		task->sched->schedule(*task);
	}
}

void flib::atomic_counter::wait_for_value(uint32_t target)
{
	while (m_cnt > target)
	{
		if (flib::fiber_util::flIsFiber())
		{
			auto fdat = flib::fiber::get_active_data();
			fdat->asleep = true;

			// sleep
			std::unique_lock<std::mutex> lock(m_waitmut);
			wait_struct w;
			w.task = fdat;
			w.target = target;

			waiters.push_back(w);
			waiters.sort([](const wait_struct& one, const wait_struct& two)
			{
				return one.target > two.target;
			});
			/*std::sort(waiters.begin(), waiters.end(), [](auto one, auto two)
			{
				return two - one;
			});*/
			lock.unlock();
			fdat->sched->yield();

			// TODO: optimize this to put it in a waiting queue
			//flib::fiber::get_global_scheduler()->yield();
		}
		else
		{
			std::this_thread::yield();
		}
	}
}