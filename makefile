VPATH = src:headers
CXXFLAGS=-Iheaders -O2 -Wall -Wextra -Weffc++ -Wshadow -Wcast-align -Wwrite-strings -Wstrict-overflow=5  -Wpointer-arith -Winit-self  
HEADERS=ewah.h boolarray.h


unit: $(HEADERS) unit.cpp
	g++ $(CXXFLAGS) -o unit src/unit.cpp 
	
example: $(HEADERS) example.cpp
	g++ $(CXXFLAGS) -o example example.cpp

cppcheck: 
	cppcheck --enable=all headers/*.h src/*.cpp *.cpp


all: 
	unit 

doxygen: 
	doxygen doxyconfig.txt

package: 
	zip -9 EWAHBoolArray_`date +%Y-%m-%d`.zip README makefile example.cpp headers/*.h src/*.cpp
clean: 
	rm -f *.o unit example
