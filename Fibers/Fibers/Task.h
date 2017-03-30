#pragma once
#include "task_scheduler.h"

namespace flib
{
	typedef void(*TaskFunction)(flib::task_scheduler* pool, void *arg);
	struct Task
	{
		TaskFunction func;
		void *arg;
	};
}