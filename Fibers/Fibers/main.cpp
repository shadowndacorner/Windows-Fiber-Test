#include <iostream>
#include "thread_ex.h"
#include "thread_pool.h"
#include "tagged_linear_allocator.h"
#include "atomic_counter.h"
//#include "fiber_util.h"
//#include "fiber_scheduler.h"
#include "tasks.h"
#include "fiber_scheduler.h"
using namespace flib;
atomic_counter frame_cnt;

int main(int argc, char** argv, char** env)
{
	flib::fiber_util::flInitializeSystem();
	
	// ensure that the main thread runs different logic from the workers
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


	// Fiber scheduler test
	struct fiber_job {
		flib::fiber::fiber_scheduler* sched;
		bool* running;
	};

	bool brunning = true;
	fiber_job data;
	data.sched = flib::task_scheduling::task_scheduler();
	data.running = &brunning;
	for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
	{
		pool.post_job([](void* data) {
			fiber_job dat = *reinterpret_cast<fiber_job*>(data);
			while (*(dat.running))
			{
				dat.sched->do_work();
			}

		}, &data, NULL);
	}

	/*
	if (false){
		bool running = true;
		std::vector<fiber_job> jobs;
		flib::fiber::fiber_scheduler& sched = *flib::fiber::get_global_scheduler();
		flib::atomic_counter g_cnt;
		for (int i = 0; i < 100; ++i)
		{
			flib::task_decl task;
			task.data = nullptr;
			task.prio = flib::task_priority::low;
			task.func = TASK_LAMBDA
			{
				auto cur = flib::fiber_util::flGetCurrentFiber();
				printf("Running fiber 0x%p, yielding...\n", cur);
				sched->yield();

				printf("Spawning child task...\n");
				flib::atomic_counter cnt;
				for (int i = 0; i < 1; ++i)
				{
					flib::task_decl child;
					child.data = nullptr;
					child.prio = flib::task_priority::high;
					child.func = TASK_LAMBDA{
						// do a lot of shit
						printf("Child fiber spinning for a long fucking time\n");
						for (int i = 0; i < 10000; ++i)
						{
							if (i % 1000 == 0)
								printf("yielding...\n");
							sched->yield();
						}
						printf("Done spinning, the other fiber should be able to resume now\n");
					};
					sched->run_jobs(&child, 1, &cnt);
				}
				printf("Waiting on child task to complete...\n");
				cnt.wait_for_value(0);
				printf("Child completed!  [0x%p\n", cur);
			};
			task.prio = flib::task_priority::high;
			sched.run_jobs(&task, 1, &g_cnt);
		}

		fiber_job data;
		data.sched = flib::fiber::get_global_scheduler();
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

		while (g_cnt > 0)
		{
			sched.do_work();
		}

		printf("Press enter to kill fiber queue...\n");
		std::cin.getline(f, 512);
		running = false;
	}
	*/
	{
		using namespace flib;

		atomic_counter gcnt;
		auto frame_task =
			TASK_LAMBDA
		{
		using namespace flib;
		printf("sched @ %p, data @ %p\n", sched, data);

		task_decl child;
		child.func = TASK_LAMBDA{
			using namespace flib;
			for (int i = 0; i < 10000; ++i)
			{
				if (i % 50 == 0)
					this_task::yield();
			}

			for (int i = 0; i < 50; ++i)
			{
				task_decl child;
				child.func = TASK_LAMBDA{
					using namespace flib;
					for (int i = 0; i < 10000; ++i)
					{
						if (i % 50 == 0)
							this_task::yield();
					}
				};
				child.prio = task_priority::low;
				RunTask(child, &frame_cnt);
			}
			WaitForCounter(&frame_cnt, 5);
		};
		child.prio = flib::task_priority::low;
		
		RunTask(child, &frame_cnt);
		// Kick off gameplay coroutine
		frame_cnt.wait_for_value(0);
		
		// Kick off rendering coroutine
		frame_cnt.wait_for_value(0);
		
		// Kick off GPU coroutine
		frame_cnt.wait_for_value(0);
		};

		task_decl task;
		task.func = frame_task;
		task.prio = task_priority::high;
		task.data = &gcnt;
		RunTask(task, &gcnt);
		while (gcnt > 0)
		{
			flib::fiber::get_global_scheduler()->do_work();
		}
	}

	std::cin.getline(f, 512);
	brunning = false;

	printf("All tests completed, press enter to clear memory\n");
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