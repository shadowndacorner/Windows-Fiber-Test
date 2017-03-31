#include "fiber_util.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

fid_t flib::fiber_util::flInitializeThread()
{
	return ConvertThreadToFiber(0);
}

void flib::fiber_util::flFiberSwitch(const fid_t& fid)
{
	SwitchToFiber((LPVOID)fid);
}

fid_t flib::fiber_util::flCreateFiber(const size_t& stack_size, const fib_start& start)
{
	return CreateFiber(stack_size, start, 0);
}

fid_t flib::fiber_util::flGetCurrentFiber()
{
	return GetCurrentFiber();
}

void flib::fiber_util::flDeleteFiber(const fid_t& fid)
{
	DeleteFiber(fid);
}