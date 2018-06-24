.PHONY: clean
CC = gcc
CFLAGS = -g -Wall -lpthread

all: httpd

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

httpd: httpd.o read_config.o handler.o
	$(CC) $(CFLAGS) -o $@ $+

clean:
	rm -f *.o httpd

