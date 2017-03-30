CXX=g++
CPPFLAGS=-g -pthread -std=c++11
LDFLAGS=-g

main:
	$(CXX) $(CPPFLAGS) main.cpp thread_pool.cpp -o test.out
	./test.out
	
clean:
	rm -f *.out