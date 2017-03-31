CXX=g++
CPPFLAGS=-g -pthread -std=c++1y
LDFLAGS=-g

main: main.cpp thread_pool.cpp tagged_heap.cpp thread_pool.h thread_ex.h
	$(CXX) $(CPPFLAGS) main.cpp thread_pool.cpp tagged_heap.cpp -o test.out
	./test.out
	
clean:
	rm -f *.out