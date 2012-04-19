VPATH = src:headers
CXXFLAGS=-Iheaders -O2 -Wall -Wextra -Weffc++ -Wconversion -Wshadow -Wcast-align -Wwrite-strings -Wstrict-overflow=5  -Wpointer-arith -Winit-self  
HEADERS=ewah.h boolarray.h

all: unit unit32bits example

unit32bits: $(HEADERS) unit.cpp
	g++ $(CXXFLAGS) -m32 -o unit32bits src/unit.cpp 
	
unit: $(HEADERS) unit.cpp 
	g++ $(CXXFLAGS) -o unit src/unit.cpp 
		
example: $(HEADERS) example.cpp
	g++ $(CXXFLAGS) -o example example.cpp

cppcheck: 
	cppcheck --enable=all headers/*.h src/*.cpp *.cpp


doxygen: 
	doxygen doxyconfig.txt

package: 
	zip -9 EWAHBoolArray_`date +%Y-%m-%d`.zip README CHANGELOG makefile example.cpp headers/*.h src/*.cpp
	zip -9 EWAHBoolArray.0.1.0-src.zip README CHANGELOG makefile example.cpp headers/*.h src/*.cpp
clean: 
	rm -f *.o unit example unit32bits
