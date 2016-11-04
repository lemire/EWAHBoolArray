VPATH = src:headers
ifeq ($(DEBUG),1)
CXXFLAGS=-Iheaders -std=c++0x -O3 -ggdb -D_GLIBCXX_DEBUG -Wall -Wextra   -Wshadow  -Wcast-qual -Wwrite-strings  -Wpointer-arith -Winit-self  -fsanitize=undefined -ftrapv
else
CXXFLAGS=-Iheaders -std=c++0x -O3 -Wall -Wextra  -Wshadow  -Wcast-qual -Wwrite-strings  -Wpointer-arith -Winit-self  
endif
HEADERS=ewah.h ewahutil.h boolarray.h runninglengthword.h

all: unit  example example2 benchmark

unit32bits: $(HEADERS) unit.cpp
	$(CXX) $(CXXFLAGS) -m32 -o unit32bits src/unit.cpp 
	
unit: $(HEADERS) unit.cpp 
	$(CXX) $(CXXFLAGS) -o unit src/unit.cpp 
		
example: $(HEADERS) example.cpp
	$(CXX) $(CXXFLAGS) -o example example.cpp

example2: $(HEADERS) example2.cpp
	$(CXX) $(CXXFLAGS) -o example2 example2.cpp

example3: $(HEADERS) example3.cpp
	$(CXX) $(CXXFLAGS) -o example3 example3.cpp


cppcheck: 
	cppcheck --enable=all headers/*.h src/*.cpp *.cpp

benchmark: $(HEADERS) ./src/benchmark.cpp
	$(CXX) $(CXXFLAGS) -o benchmark ./src/benchmark.cpp


doxygen: 
	doxygen doxyconfig.txt

package: 
	zip -9 EWAHBoolArray_`date +%Y-%m-%d`.zip README CHANGELOG makefile example.cpp example2.cpp headers/*.h src/*.cpp
	cd ..;zip -9 ./EWAHBoolArray/EWAHBoolArray.0.4.0-src.zip ./EWAHBoolArray/README ./EWAHBoolArray/CHANGELOG ./EWAHBoolArray/makefile ./EWAHBoolArray/example.cpp ./EWAHBoolArray/example2.cpp ./EWAHBoolArray/headers/*.h ./EWAHBoolArray/src/*.cpp
	echo "don't forget to run git tag 0.4.0 and do git push --tags"
clean: 
	rm -f *.o unit example example2 unit32bits benchmark
