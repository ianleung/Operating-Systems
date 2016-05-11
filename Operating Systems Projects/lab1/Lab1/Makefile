C=gcc
CFLAGS=-Wall -g
LD=gcc
LDFLAGS=-g


OBJS=myls.o

all: myls.out

myls.out: $(OBJS) 
	$(LD) $(LDFLAGS) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o *.out
