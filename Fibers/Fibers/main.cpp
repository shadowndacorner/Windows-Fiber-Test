#include <iostream>
#include "thread_pool.h"
#include "tagged_heap.h"

int main(int argc, char** argv, char** env)
{
	flib::tagged_heap& heap = flib::shared_heap;
	flib::thread_pool pool;
	char f[512];
	std::cin.getline(f, 512);
	std::cout << "Exiting...\n";

	heap.alloc_block(1);
	heap.alloc_block(1);
	heap.alloc_block(2);
	heap.alloc_block(1);
	heap.alloc_block(3);
	heap.alloc_block(1);
	heap.free(1);
	heap.alloc_block(2);
	heap.alloc_block(2);
	heap.alloc_block(3);
	heap.alloc_block(1);
	heap.alloc_block(2);
	heap.alloc_block(3);
	heap.alloc_block(3);
}