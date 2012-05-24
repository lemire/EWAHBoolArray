VPATH = src:headers
CXXFLAGS=-Iheaders -O3 -Wall -Wextra -Weffc++ -Wconversion -Wshadow -Wcast-align -Wcast-qual -Wwrite-strings -Wstrict-overflow=5 -Wunsafe-loop-optimizations  -Wpointer-arith -Winit-self  
HEADERS=ewah.h ewahutil.h boolarray.h runninglengthword.h

all: unit unit32bits example benchmark

unit32bits: $(HEADERS) unit.cpp
	$(CXX) $(CXXFLAGS) -m32 -o unit32bits src/unit.cpp 
	
unit: $(HEADERS) unit.cpp 
	$(CXX) $(CXXFLAGS) -o unit src/unit.cpp 
		
example: $(HEADERS) example.cpp
	$(CXX) $(CXXFLAGS) -o example example.cpp

cppcheck: 
	cppcheck --enable=all headers/*.h src/*.cpp *.cpp

benchmark: $(HEADERS) ./src/benchmark.cpp
	$(CXX) $(CXXFLAGS) -o benchmark ./src/benchmark.cpp


doxygen: 
	doxygen doxyconfig.txt

package: 
	zip -9 EWAHBoolArray_`date +%Y-%m-%d`.zip README CHANGELOG makefile example.cpp headers/*.h src/*.cpp
	cd ..;zip -9 ./EWAHBoolArray/EWAHBoolArray.0.2.1-src.zip ./EWAHBoolArray/README ./EWAHBoolArray/CHANGELOG ./EWAHBoolArray/makefile ./EWAHBoolArray/example.cpp ./EWAHBoolArray/headers/*.h ./EWAHBoolArray/src/*.cpp
clean: 
	rm -f *.o unit example unit32bits
