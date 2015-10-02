CC=g++
LIBS= -lglut -lGLEW -lGL -lassimp

CXXFLAGS= -g -Wall -std=c++0x

all:	../bin/Matrix

../bin/Matrix:	main.o shaderClass.o loaderClass.o
	$(CC) $(CXXFLAGS) -o ../bin/Matrix main.o shaderClass.o loaderClass.o $(LIBS)

main.o:	../src/main.cpp
	$(CC) $(CXXFLAGS) -c ../src/main.cpp $(LIBS)

shaderClass.o:	../src/shaderClass.cpp ../src/shaderClass.h
	$(CC) $(CXXFLAGS) -c ../src/shaderClass.cpp $(LIBS)

loaderClass.o:	../src/loaderClass.cpp ../src/loaderClass.h
	$(CC) $(CXXFLAGS) -c ../src/loaderClass.cpp $(LIBS)

clean:
	\rm *.o *~ ../bin/Matrix
