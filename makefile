PREFIX?=/usr/local

all : example test

test : cl_options.h test.cpp
	$(CXX) -std=c++11 test.cpp -o test

example : cl_options.h example.cpp
	$(CXX) -std=c++11 example.cpp -o example

clean : 
	rm -f test example

.PHONY : all clean install uninstall

install :
	cp cl_options.h $(PREFIX)/include/

uninstall :
	rm $(PREFIX)/include/cl_options.h
