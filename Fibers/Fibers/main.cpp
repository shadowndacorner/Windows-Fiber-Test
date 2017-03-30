#include "thread_ex.h"
#include <iostream>
#include <vector>

static bool running = true;

void thread_func(int threadid)
{
	// Wait until the thread affinity was successfully set
	while (get_current_processor() != threadid)
	{
		std::cout << "Yielding thread " << threadid << " until get processor (currently on "<<get_current_processor()<<")\n";
		std::this_thread::yield();
	}

	std::cout << "Running thread " << threadid << "\n";
	while (running)
	{
		auto aff = get_current_processor();
		if (aff != threadid)
		{
			std::cout << "ERROR!  Thread " << threadid << " running on processor " << aff << "!\n";
		}
		else
		{
			std::cout << "Thread " << threadid << "\n";
		}
		std::this_thread::yield();
	}
}

int main(int argc, char** argv, char** env)
{
	using namespace std;
	vector<thread> thrds;
	std::cout << "Initializing...\n";
	for (unsigned int i = 0; i < thread::hardware_concurrency(); ++i)
	{
		thrds.push_back(thread(thread_func, i));
		thread& thr = thrds.back();
		set_thread_affinity(thr, i);
	}
	
	std::cout << "All threads initialized, running...\n";
	char f[512];
	std::cin.getline(f, 512);
	
	running = false;
	for (auto x = thrds.begin(); x < thrds.end(); ++x)
	{
		(*x).join();
	}

}