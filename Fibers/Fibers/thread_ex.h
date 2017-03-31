#pragma once
#include <thread>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <pthread.h>
#include <sched.h>
#endif

static int set_thread_affinity(std::thread& thr, const int& core)
{
#ifdef _WIN32
	if (sizeof(int) != sizeof(void*))
		return (int)SetThreadAffinityMask((HANDLE)thr.native_handle(), (1i64 << core));
	else
		return (int)SetThreadAffinityMask((HANDLE)thr.native_handle(), (1i64 << core));
#else
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core + 1, &cpuset);

	pthread_t current_thread = pthread_self();
	return (int)pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
#endif
}

static int get_current_processor()
{
#ifdef _WIN32
	return GetCurrentProcessorNumber();
#else
	return sched_getcpu();
#endif
}

static int set_this_thread_affinity(const int& core)
{
#ifdef _WIN32
	auto ret = (int)SetThreadAffinityMask(GetCurrentThread(), (1i64 << core));
#else
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core + 1, &cpuset);

	pthread_t current_thread = pthread_self();
	auto ret = (int)pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
#endif
	while (get_current_processor() != core)
	{
		std::this_thread::yield();
	}

	return ret;
}