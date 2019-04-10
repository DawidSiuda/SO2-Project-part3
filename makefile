CC=g++
DEFINES = 
CFLAGS = -std=c++11 
LIB = -lGL -lGLU -lglut -L/usr/lib/x86_64-linux-gnu -lm -lpthread

etap1: clean Ball.o main.o
	$(CC) -o etap1  main.o Ball.o $(LIB)

main.o: main.cpp Ball.h
	$(CC) -c $(DEFINES) $(CFLAGS) main.cpp -o main.o 

Ball.o: Ball.cpp Ball.h
	$(CC) -c $(DEFINES) $(CFLAGS) Ball.cpp -o Ball.o

clean:
	-rm *.o

