#include <iostream>
#include "thread_ex.h"
#include "thread_pool.h"
#include "tagged_linear_allocator.h"
#include "atomic_counter.h"
#include "fiber_util.h"
#include "fiber_scheduler.h"

int main(int argc, char** argv, char** env)
{
	flib::thread_pool pool(std::thread::hardware_concurrency() - 1);
	set_this_thread_affinity(std::thread::hardware_concurrency() - 1);

	char f[512];
	std::cin.getline(f, 512);
	struct allocTest
	{
		char arr[1024];
	};
	
	// Bruteforce test
	for (int i = 0; i < 0; ++i)
	{
		for (int i = 0; i < 100; ++i)
		{
			flib::shared_heap.free(i);
		}

		flib::atomic_counter count;
		count = 0;
		for (int i = 0; i < 1000; ++i)
		{
			pool.post_job([](void* const data) {
				int i = (int)data;
				printf("job %d\n", i);
				flib::tagged_linear_allocator<allocTest> alloc(10);
				for (int i = 0; i < 1000; ++i)
				{
					alloc.allocate();
				}

				if (i % 3 == 0)
				{
					flib::shared_heap.free(10 + i % 10);
				}
			}, (void*)i, &count);
		}

		while (count > 0) {
			if (pool.do_work_if_available())
			{
				printf("Main thread working...\n");
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}

	// Fiber test
	{
		using namespace flib;
		atomic_counter cnt;
		cnt = 0;
		for (int i = 0; i < 1; ++i)
		{
			pool.post_job([](void* dat)
			{
				using namespace flib::fiber_util;
				std::vector<fid_t> ids;

				auto main = flInitializeThread();
				auto fone = flCreateFiber(512, FIBER_LAMBDA(
					auto& ids = *reinterpret_cast<std::vector<fid_t>*>(data);
					printf("Should happen first\n");
					flFiberSwitch(ids[2]);
				), &ids);

				auto ftwo = flCreateFiber(512, [](void* data)
				{
					auto& ids = *reinterpret_cast<std::vector<fid_t>*>(data);
					printf("Should happen second\n");
					flFiberSwitch(ids[0]);
					printf("Should happen fourth\n");
					flFiberSwitch(ids[0]);
				}, &ids);

				ids.push_back(main);
				ids.push_back(fone);
				ids.push_back(ftwo);
				flFiberSwitch(fone);
				printf("Should happen third\n");
				flFiberSwitch(ftwo);
				printf("Fibers completed\n");

				flDeleteFiber(fone);
				flDeleteFiber(ftwo);

				if (flDeinitializeThread())
					printf("success\n");
				else
					printf("fail?\n");

			}, NULL, &cnt);
		}

		while (cnt > 0)
		{
			std::this_thread::yield();
		}
	}

	struct fiber_job {
		flib::fiber::fiber_scheduler* sched;
		bool* running;
	};

	bool running = true;
	std::vector<fiber_job> jobs;
	flib::fiber::fiber_scheduler sched;

	for (int i = 0; i < 10; ++i)
	{
		flib::fiber::task_decl task;
		task.data = nullptr;
		task.func = TASK_LAMBDA
		{
			auto cur = flib::fiber_util::flGetCurrentFiber();
			printf("Running fiber 0x%p, yielding...\n", cur);
			sched->yield();
			printf("Rescheduled 0x%p\n", cur);
		};
		task.prio = flib::fiber::fiber_priority::high;
		sched.run_jobs(&task, 1, nullptr);
	}

	fiber_job data;
	data.sched = &sched;
	data.running = &running;
	for (int i = 0; i < std::thread::hardware_concurrency() - 1; ++i)
	{
		pool.post_job([](void* data) {
			fiber_job dat = *reinterpret_cast<fiber_job*>(data);
			while (*(dat.running))
			{
				dat.sched->do_work();
			}

		}, &data, NULL);
	}

	printf("Press enter to kill fiber queue...\n");
	std::cin.getline(f, 512);
	running = false;

	printf("All jobs finished, press enter to clear memory\n");
	std::cin.getline(f, 512);

	for (int i = 0; i < 20; ++i)
	{
		flib::shared_heap.free(i);
	}
	printf("Press enter to close\n");
	auto& heap = flib::shared_heap;
	std::cin.getline(f, 512);
	printf("Finishing\n");
}