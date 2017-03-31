#pragma once
#include <queue>
#include "fiber_util.h"

namespace flib
{
	namespace fiber
	{
		class fiber_scheduler
		{
		private:
			std::queue<fid_t> high_prio;
			std::queue<fid_t> med_prio;
			std::queue<fid_t> low_prio;
		};
	}
}