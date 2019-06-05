CC=g++
DEFINES = 
CFLAGS = -std=c++11 
LIB = -lGL -lGLU -lglut -L/usr/lib/x86_64-linux-gnu -lm -lpthread

etap1: clean Ball.o Bat.o main.o
	$(CC) -o etap1  main.o Ball.o Bat.o $(LIB)

main.o: main.cpp Ball.h Bat.h
	$(CC) -c -g $(DEFINES) $(CFLAGS) main.cpp -o main.o 

Ball.o: Ball.cpp Ball.h
	$(CC) -c -g $(DEFINES) $(CFLAGS) Ball.cpp -o Ball.o

Bat.o: Bat.cpp Bat.h
	$(CC) -c -g $(DEFINES) $(CFLAGS) Bat.cpp -o Bat.o

clean:
	-rm -f *.o

