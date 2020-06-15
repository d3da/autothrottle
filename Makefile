CC=gcc
CFLAGS=-g -I.

all:
	$(CC) -o autothrottle autothrottle.c config.c $(CFLAGS)

install:
	cp autothrottle /usr/local/bin/

clean:
	rm autothrottle
