CCC=g++

COPTS=-m32 -Wall -g -I./msieve/include
LOPTS=-m32 -L./msieve -lm -lgmp -lmsieve -lpthread -lz

SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

MAIN=kry

all: $(MAIN)
	

run: ${MAIN}
	./${MAIN}


$(MAIN): $(OBJECTS)
	cd msieve; make x86
	$(CCC) $(OBJECTS) $(LOPTS) -o $(MAIN)

%.o:%.cpp
	$(CCC) -c $(COPTS) $< -o $@

clean:
	rm *.o $(MAIN)
	cd msieve; make clean