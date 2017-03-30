CXX=g++
CPPFLAGS=-g -pthread -std=c++11
LDFLAGS=-g

main: main.cpp thread_pool.cpp thread_pool.h thread_ex.h
	$(CXX) $(CPPFLAGS) main.cpp thread_pool.cpp -o test.out
	./test.out
	
clean:
	rm -f *.out