#pragma once
#include <thread>

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#include <sched.h>
#endif

static int set_thread_affinity(std::thread& thr, const int& core)
{
#ifdef _WIN32
	return (int)SetThreadAffinityMask((HANDLE)thr.native_handle(), (1 << core));
#else
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core, &cpuset);

	pthread_t current_thread = pthread_self();
	return (int)pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
#endif
}

static int get_current_processor()
{
#ifdef _WIN32
	return GetCurrentProcessorNumber();
#else
	return sched_getcpu() + 1;
#endif
}