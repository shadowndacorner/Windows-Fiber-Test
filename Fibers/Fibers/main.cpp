#include "thread_ex.h"
#include <iostream>
#include <vector>

static bool running = true;

void thread_func(int threadid)
{
	// Wait until the thread affinity was successfully set
	while (get_current_processor() != threadid)
		std::this_thread::yield();

	while (running)
	{
		auto aff = get_current_processor();
		if (aff != threadid)
		{
			std::cout << "ERROR!  Thread " << threadid << " running on processor " << aff << "!\n";
		}
		std::this_thread::yield();
	}
}

int main()
{
	using namespace std;
	vector<thread> thrds;
	for (unsigned int i = 0; i < thread::hardware_concurrency(); ++i)
	{
		thrds.push_back(thread(thread_func, i));
		thread& thr = thrds.back();
		set_thread_affinity(thr, i);
	}

	char f[512];
	std::cin.getline(f, 512);
	
	running = false;
	for (auto x = thrds.begin(); x < thrds.end(); ++x)
	{
		(*x).join();
	}

}