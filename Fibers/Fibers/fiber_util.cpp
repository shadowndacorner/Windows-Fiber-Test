#include "fiber_util.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#ifdef _WIN32
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
	// TODO: for linux port, use makecontext w/ the double_int stuff at the top of the file to encode the void*
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

#else
// TODO: implement unix versions of the functions w/ makecontext/setcontext

// BEGIN LOCAL UTILITY FOR LINUX
struct double_int
{
	double_int() = default;
	double_int(int a, int b)
	{
		one = a;
		two = b;
	}

	int one;
	int two;
};


double_int ptr_to_double_int(void* const p)
{
	union
	{
		double_int d;
		void* p;
	} x;

	x.d = double_int(0, 0);
	x.p = p;

	return x.d;
}

void* double_int_to_ptr(const double_int& di)
{
	union
	{
		double_int d;
		void* p;
	} x;

	x.p = nullptr;
	x.d = di;
	return x.p;
}

void* double_int_to_ptr(const int& a, const int& b)
{
	return double_int_to_ptr(double_int(a, b));
}
// END LOCAL UTILITY FOR LINUX
#endif