#include <iostream>
#include "thread_pool.h"
#include "tagged_linear_allocator.h"

int main(int argc, char** argv, char** env)
{
	flib::thread_pool pool;
	char f[512];
	std::cin.getline(f, 512);
	std::cout << "Exiting...\n";

	struct allocTest
	{
		union
		{
			char arr[512];
			double d;
			float f;
			int num;
			char c;
		};
	};


	flib::tagged_linear_allocator<allocTest, 10> alloc;
	flib::tagged_linear_allocator<allocTest, 11> allocb;
	flib::tagged_linear_allocator<allocTest, 10> allocc;
	flib::tagged_linear_allocator<allocTest, 12> allocd;
	for (int i = 0; i < 1000000; ++i)
	{
		auto test = alloc.allocate();
	}

	for (int i = 0; i < 10000000; ++i)
	{
		auto test = allocb.allocate();
	}

	flib::shared_heap.free(11);
	for (int i = 0; i < 10000000; ++i)
	{
		auto test = allocc.allocate();
	}
	flib::shared_heap.free(10);

	for (int i = 0; i < 10000000; ++i)
	{
		auto test = allocd.allocate();
	}

	auto& heap = flib::shared_heap;
	printf("done!\n");
}