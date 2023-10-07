CXX = g++
CXXFLAGS = -g -std=c++14 -Wall -Wextra -pedantic

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $*.o

CXX_SRCS = main.cpp cache_funcs.cpp

OBJS = $(CXX_SRCS:.cpp=.o)

csim : $(OBJS) 
	make depend
	$(CXX) -o $@ $(OBJS) -lm -lz

clean :
	rm -f *.o csim

depend :
	$(CXX) -M $(CXXFLAGS) $(CXX_SRCS) >> depend.mak

depend.mak :
	touch $@

include depend.mak