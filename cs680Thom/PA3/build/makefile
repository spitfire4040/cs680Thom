CC=g++
LIBS= -lglut -lGLEW -lGL

CXXFLAGS= -g -Wall -std=c++0x

all:	../bin/Matrix

../bin/Matrix:	main.o shaderClass.o
	$(CC) $(CXXFLAGS) -o ../bin/Matrix main.o shaderClass.o $(LIBS)

main.o:	../src/main.cpp
	$(CC) $(CXXFLAGS) -c ../src/main.cpp $(LIBS)

shaderClass.o:	../src/shaderClass.cpp ../src/shaderClass.h
	$(CC) $(CXXFLAGS) -c ../src/shaderClass.cpp $(LIBS)

clean:
	\rm *.o *~ ../bin/Matrix