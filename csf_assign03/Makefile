CXX = g++
CXXFLAGS = -g -std=c++14 -Wall -Wextra -pedantic

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $*.o

CXX_SRCS = main.cpp

OBJS = $(CXX_SRCS:.cpp=.o)

main : $(OBJS) 
	make depend
	$(CXX) -o $@ $(OBJS) -lm -lz

clean :
	rm -f *.o main

depend :
	$(CXX) -M $(CXXFLAGS) $(CXX_SRCS) >> depend.mak

depend.mak :
	touch $@

include depend.mak