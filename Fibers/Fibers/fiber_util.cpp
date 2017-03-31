#include "fiber_util.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

static fid_t nul_fiber;
void flib::fiber_util::flInitializeSystem()
{
	nul_fiber = flGetCurrentFiber();
}
fid_t flib::fiber_util::flInitializeThread()
{
	return ConvertThreadToFiber(0);
}

bool flib::fiber_util::flDeinitializeThread()
{
	return ConvertFiberToThread();
}

void flib::fiber_util::flFiberSwitch(const fid_t& fid)
{
	SwitchToFiber((LPVOID)fid);
}

bool flib::fiber_util::flIsFiber()
{
	return flGetCurrentFiber() != nul_fiber;
}

fid_t flib::fiber_util::flCreateFiber(const size_t& stack_size, const fib_start& start, void* const data)
{
	return CreateFiber(stack_size, start, data);
}

fid_t flib::fiber_util::flGetCurrentFiber()
{
	return GetCurrentFiber();
}

void flib::fiber_util::flDeleteFiber(const fid_t& fid)
{
	DeleteFiber(fid);
}