# ECE254 lab4
# Makefile: build producer and consumer

CC=gcc
CFLAGS=-Wall -g
LD=gcc
LDFLAGS=-g

OBJS1=produce.o

all: produce

produce: $(OBJS1) 
	$(LD) $(LDFLAGS) -o $@ $(OBJS1) -lpthread -lm

produce.o: produce.c
	$(CC) $(CFLAGS) -c produce.c 

.PHONY: clean
clean:
	rm -f *.o *.out 
