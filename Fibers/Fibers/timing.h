#pragma once
#include <chrono>

namespace flib
{
	void Initialize();
	double GetSystemTime();

	class Timer
	{
	public:
		Timer();
		~Timer();

		double get_time();
	private:
		std::chrono::high_resolution_clock::time_point m_start;
	};
}