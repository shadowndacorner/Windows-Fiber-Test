#include <iostream>
#include "thread_ex.h"
#include "thread_pool.h"
#include "tagged_linear_allocator.h"
#include "atomic_counter.h"
//#include "fiber_util.h"
//#include "fiber_scheduler.h"
#include "tasks.h"
#include "fiber_scheduler.h"
#include "timing.h"
#include "frame_data.h"
#include "ska_sort.h"
#define MICROPROFILE_EMBED_HTML
#include "microprofile/microprofile.h"

using namespace flib;
static bool is_game_running = true;

TASK_FUNCTION(frame_task)
{
	MICROPROFILE_SCOPEI("frametime", "frame", 0x308080);
	uint64_t dat = (uint64_t)data;
	uint32_t sdat = (uint32_t)dat;

	Timer frame_time;
	atomic_counter* frame_cnt = nullptr;
	if (dat == 0)
		InitializeFrame(sdat, 1.0 / 60.0);
	else
		InitializeFrame(sdat, GetFrameData(sdat - 1)->deltaTime);

	// validate setup
	auto frame_data = GetFrameData((uint32_t)dat);
	if (frame_data->m_frameNum != dat)
		throw std::invalid_argument("Returned invalid frame");

	{
		MICROPROFILE_SCOPEI("stages", "game_logic", 0x080880ff);
		task_decl child;
		child.func = TASK_LAMBDA{
			atomic_counter* m_waiter = nullptr;
			{
				using namespace flib;
				for (int i = 0; i < 10000; ++i)
				{
					//if (i % 50 == 0)
						//this_task::yield();
				}

				for (int i = 0; i < 50; ++i)
				{
					task_decl child;
					child.func = TASK_LAMBDA{
						MICROPROFILE_SCOPEI("stages", "game_logic", 0x080880ff);
						using namespace flib;
						for (int i = 0; i < 10000; ++i)
						{
							//if (i % 900 == 0)
								//this_task::yield();
						}
					};
					child.prio = task_priority::high;
					RunTask(child, &m_waiter);
				}
				MICROPROFILE_SCOPEI("stages", "game_logic", 0x080880ff);
			}
			WaitForCounter(m_waiter, 0);
		};
		child.prio = flib::task_priority::high;
		child.data = 0;

		RunTask(child, &frame_cnt);

		// Kick off gameplay coroutine
	}
	frame_cnt->wait_for_value(0);

	{
		MICROPROFILE_SCOPEI("stages", "render", 0x080880ff);

		// Kick off rendering coroutine
	}
	frame_cnt->wait_for_value(0);

	{
		MICROPROFILE_SCOPEI("stages", "gpu_submit", 0x080880ff);

		// Kick off GPU coroutine
	}
	frame_cnt->wait_for_value(0);	// main thread?
	//printf("Done with the shit\n");

	// flip
	// graphics_context->flip();
	frame_data->m_hasRendered = true;
	MicroProfileFlip(nullptr);

	// Free per-frame globals
	shared_heap().free(dat);

	double dt = frame_time.get_time();
	frame_data->deltaTime = dt;
	printf("Frame %llu complete in %lf seconds\n", dat, dt);

	if (is_game_running)
	{
		task_decl task;
		task.func = frame_task;
		task.data = (void*)(dat + 1);
		RunTask(task, nullptr);
	}
}

int main(int argc, char** argv, char** env)
{
	MicroProfileOnThreadCreate("Main");

	MicroProfileSetEnableAllGroups(true);

	MicroProfileSetForceMetaCounters(true);



	MicroProfileStartContextSwitchTrace();
	// ska_sort example.  this is totally unnecesary
	std::vector<int> ex{53, 665, 3334, 6, 49, 123, 654, 23, 5050, 19293, 49302, 120394, 449, 654, 345, 7876, 432 ,45678, 87,432, 34567, 87654, 324567, 8};
	ska_sort(ex.begin(), ex.end(), [](const int& v)
	{
		return v;
	});

	flib::fiber_util::flInitializeSystem();
	
	flib::thread_pool pool(2);
	// ensure that the main thread runs different logic from the workers
	//set_this_thread_affinity(std::thread::hardware_concurrency() - 1);

	char f[512];
	std::cin.getline(f, 512);
	struct allocTest
	{
		char arr[1024];
	};
	
	// Fiber scheduler test
	struct fiber_job {
		flib::fiber::fiber_scheduler* sched;
		bool* running;
	};

	bool brunning = true;
	for (uint32_t i = 0; i < std::thread::hardware_concurrency(); ++i)
	{
		pool.post_job([](void* data) {
			bool* dat = reinterpret_cast<bool*>(data);
			auto sched = flib::task_scheduling::task_scheduler();
			while (*(dat))
			{
				sched->do_work();
			}
		}, &brunning, NULL);
	}

	task_decl task;
	task.func = frame_task;
	task.prio = task_priority::low;
	task.data = 0;
	RunTask(task, nullptr);

	std::cin.getline(f, 512);
	brunning = false;
	is_game_running = false;

	printf("All tests completed, press enter to exit\n");

	std::cin.getline(f, 512);
	printf("Closing...\n");
	MicroProfileShutdown();
}