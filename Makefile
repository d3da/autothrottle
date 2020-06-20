CC=gcc
CFLAGS=-g -I. -Wall -ggdb

all:
	$(CC) -o autothrottle autothrottle.c config.c $(CFLAGS)

install:
	cp autothrottle /usr/local/bin/
	cp -n autothrottle.conf /etc/

clean:
	rm autothrottle

