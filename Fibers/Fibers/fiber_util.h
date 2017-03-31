#pragma once
#include <stdint.h>
#include <functional>

#ifdef _WIN32
#include <Windows.h>
typedef LPVOID fid_t;
typedef LPFIBER_START_ROUTINE fib_start;
#endif

namespace flib
{
	namespace fiber_util
	{
		fid_t flInitializeThread();
		fid_t flCreateFiber(const size_t&, const fib_start&);
		fid_t flGetCurrentFiber();
		void flFiberSwitch(const fid_t&);
		void flDeleteFiber(const fid_t&);
	}
}