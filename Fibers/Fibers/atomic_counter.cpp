#include "atomic_counter.h"
#include "fiber_scheduler.h"

flib::atomic_counter::atomic_counter()
	:alloc(nullptr)
{
	m_cnt = 0;
}

flib::atomic_counter::atomic_counter(std::allocator<atomic_counter>*cnt)
	:alloc(cnt)
{
	m_cnt = 0;
}

flib::atomic_counter::atomic_counter(const uint32_t & cnt)
	: alloc(nullptr)
{
	m_cnt = cnt;
}

flib::atomic_counter::atomic_counter(const atomic_counter &cnt)
	: alloc(nullptr)
{
	m_cnt = cnt.m_cnt.load();
}

void flib::atomic_counter::resume_waiters()
{
	/*std::unique_lock<std::mutex> lock(m_waitmut);
	for (auto iter = waiters.begin(); iter != waiters.end(); ++iter)
	{
		if ((*iter).target > m_cnt)
			return;

		auto task = reinterpret_cast<fiber::fiber_data*>((*iter).task);
		task->asleep = false;
		task->sched->schedule(*task);
	}*/
}

void flib::atomic_counter::wait_for_value(const uint32_t& target)
{
	while (m_cnt > target)
	{
		if (flib::fiber_util::flIsFiber())
		{
			//flib::fiber::get_global_scheduler()->yield();
			/*
			auto fdat = flib::fiber::get_active_data();
			fdat->asleep = true;

			// sleep
			std::unique_lock<std::mutex> lock(m_waitmut);
			wait_struct w;
			w.task = fdat;
			w.target = target;

			waiters.push_back(w);
			if (waiters.size() > 1)
			{
				waiters.sort([](const wait_struct& one, const wait_struct& two)
				{
					return one.target > two.target;
				});
			}
			lock.unlock();
			fdat->sched->yield();
			*/
			// TODO: optimize this to put it in a waiting queue
			//flib::fiber::get_global_scheduler()->yield();
			flib::fiber::get_active_data()->sched->yield();
		}
		else
		{
			std::this_thread::yield();
		}
	}
}