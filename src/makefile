CXX = g++
CXXFLAGS = -O3 -Wall #-pg -g 
RM = rm -f

EXE = slidingHPSS 
SRC = slidingHPSS.cpp 

## Libraries
L_FFTW3         = -lfftw3
LIBS = $(L_FFTW3) -lm

# -----------------------------------
.PHONY: all
all:
	$(CXX) $(CXXFLAGS) -o $(EXE) $(SRC) $(LIBS)

.SUFFIXES: .o .cpp
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

.PHONY: clean
clean: 
	$(RM) $(EXE) 
	$(RM) *~


