CC=gcc -Wall -W -ggdb3
CFLAGS=-I../lib -Wall -O3 -ggdb3
#LDFLAGS=-lcell -lefence
LDFLAGS=-L../lib -lcell -lm

PREFIX=/usr/local

SRC =	ads cell clwho celldemo multicell readtag rmwcell writetag



all: $(SRC)

install: all
	cp -p $(SRC) $(PREFIX)/bin

clean:
	rm -f $(SRC) *~

distclean: clean

cell-static: cell.c ../lib/libcell.a
	gcc -I ../lib -L ../lib -c cell.c
	gcc -lm -o cell-static cell.o ../lib/libcell.a

clwho-static: clwho.c ../lib/libcell.a
	gcc -I ../lib -L ../lib -c clwho.c
	gcc -lm -o clwho-static clwho.o ../lib/libcell.a

readtag-static: readtag.c ../lib/libcell.a
	gcc -I ../lib -L ../lib -c readtag.c
	gcc -lm -o readtag-static readtag.o ../lib/libcell.a

writetag-static: writetag.c ../lib/libcell.a
	gcc -I ../lib -L ../lib -c writetag.c
	gcc -lm -o writetag-static writetag.o ../lib/libcell.a
