#include <iostream>
#include "thread_ex.h"
#include "thread_pool.h"
#include "tagged_linear_allocator.h"
#include "atomic_counter.h"

int main(int argc, char** argv, char** env)
{
	flib::thread_pool pool(std::thread::hardware_concurrency() - 1);
	set_this_thread_affinity(std::thread::hardware_concurrency() - 1);

	char f[512];
	std::cin.getline(f, 512);
	struct allocTest
	{
		char arr[1024]; 
	};

	flib::atomic_counter count;
	count = 0;
	for (int i = 0; i < 1000; ++i)
	{
		pool.post_job([](void* const data){
			int i = (int)data;
			flib::tagged_linear_allocator<allocTest> alloc(10);
			for (int i = 0; i < 1000; ++i)
			{
				alloc.allocate();
			}
		}, (void*)i, &count);
	}

	while (count > 0) {
		if (pool.do_work_if_available())
		{
			printf("Main thread working...\n");
		}
		else
		{
			std::this_thread::yield();
		}
	}

	printf("All jobs finished, press enter to clear memory\n");
	std::cin.getline(f, 512);

	for (int i = 0; i < 20; ++i)
	{
		flib::shared_heap.free(i);
	}
	printf("Press enter to close\n");
	auto& heap = flib::shared_heap;
	std::cin.getline(f, 512);
	printf("Finishing\n");
}