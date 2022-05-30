TARGET=main.out
CC=gcc
WARN=-Wall
VERSION=-std=c99
PTHREAD=-lpthread
CCFLAGS= $(WARN) $(VERSION) 
OBJS= main.o

all: clean $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(PTHREAD)
 
main.o:
	$(CC) -c main.c $(CCFLAGS) -o main.o
 
clean:
	rm -f *.o main