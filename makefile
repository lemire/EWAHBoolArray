VPATH = src:headers
CXXFLAGS=-Iheaders -O2 -Wall -DNDEBUG
HEADERS=ewah.h boolarray.h


unit: $(HEADERS) unit.cpp
	g++ $(CXXFLAGS) -o unit src/unit.cpp 
	
example: $(HEADERS) example.cpp
	g++ $(CXXFLAGS) -o example example.cpp


all: 
	unit 


package: 
	zip -9 EWAHBoolArray_`date +%Y-%m-%d`.zip README makefile example.cpp headers/*.h src/*.cpp
clean: 
	rm -f *.o unit example
