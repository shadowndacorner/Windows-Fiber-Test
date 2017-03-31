#include <iostream>
#include "thread_pool.h"
#include "tagged_linear_allocator.h"
#include "atomic_counter.h"

int main(int argc, char** argv, char** env)
{
	flib::thread_pool pool;
	char f[512];
	std::cin.getline(f, 512);
	struct allocTest
	{
		char arr[1024]; 
	};

	flib::atomic_counter count;
	count = 5;
	for (int i = 0; i < 5; ++i)
	{
		pool.post_job([i, &count]{
			flib::tagged_linear_allocator<allocTest> alloc(10 + i % 2);
			for (int i = 0; i < 1000000; ++i)
			{
				alloc.allocate();
			}

			printf("Thread %d done\n", i);

			if ((i-2) % 3 == 0) {
				printf("Freeing memory\n");
				flib::shared_heap.free(10 + (i % 2));
			}
			--count;
		});
	}

	while (count > 0) {
		std::this_thread::yield();
	}

	printf("All jobs finished, press enter to clear memory\n");
	std::cin.getline(f, 512);

	for (int i = 0; i < 15; ++i)
	{
		flib::shared_heap.free(i);
	}
	printf("Press enter to close\n");
	auto& heap = flib::shared_heap;
	std::cin.getline(f, 512);
	printf("Finishing\n");
}