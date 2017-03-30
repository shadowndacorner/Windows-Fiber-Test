#include <iostream>
#include "thread_pool.h"

int main(int argc, char** argv, char** env)
{
	flib::thread_pool pool;
	char f[512];
	std::cin.getline(f, 512);
	std::cout << "Exiting...\n";
}