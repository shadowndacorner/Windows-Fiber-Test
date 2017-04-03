#include "timing.h"

using namespace std::chrono;
static high_resolution_clock::time_point startup;
typedef std::chrono::duration<double> dsec;

void flib::Initialize()
{
	startup = high_resolution_clock::now();
}

flib::Timer::Timer()
{
	m_start = high_resolution_clock::now();
}

flib::Timer::~Timer()
{
	dsec time = high_resolution_clock::now() - m_start;
}

double flib::Timer::get_time()
{
	return (static_cast<dsec>(high_resolution_clock::now() - m_start)).count();
}
