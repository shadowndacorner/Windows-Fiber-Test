#pragma once
#include <stdint.h>
#include <functional>

#ifdef _WIN32
#include <Windows.h>
typedef LPVOID fid_t;
typedef LPFIBER_START_ROUTINE fib_start;
#endif

#define FIBER_FUNCTION(name, body) void name(void* data){body}
#define FIBER_LAMBDA(body) [](void* data){body}
namespace flib
{
	namespace fiber_util
	{
		void flInitializeSystem();
		fid_t flInitializeThread();
		bool flDeinitializeThread();
		fid_t flCreateFiber(const size_t&, const fib_start&, void* const data);
		fid_t flGetCurrentFiber();
		bool flIsFiber();
		void flFiberSwitch(const fid_t&);
		void flDeleteFiber(const fid_t&);
	}
}