# Makefile for libcell - Modified by Byron Stanoszek (5/6/2002)

SUBDIRS=lib src
CWD = "/projects/cell"
FILENAME=cell-v0.0.7e.tar.gz

all: $(SUBDIRS)

install: all
	cd lib;$(MAKE) install
	cd src;$(MAKE) install

${SUBDIRS}: dummy
	cd $@;${MAKE} all

clean:
	cd lib;$(MAKE) clean
	cd src;$(MAKE) clean
	rm -f *~

dummy: # Always force subdirectories.


beta:   clean
	cd ..; \
	tar czvf  archives/$(FILENAME) cell; \
	chown ron:users archives/$(FILENAME)

dist:   clean
	cd ..; \
	tar czvf archives/$(FILENAME) cell; \
	chown ron:users archives/$(FILENAME)

release:   clean
	cd ..; \
	tar czvf  archives/$(FILENAME) cell; \
	chown ron:users archives/$(FILENAME)

major:   clean
	awk -f major.awk -- .version > .newversion
	mv .newversion .version
	cd ..; \
	tar czvf  archives/$(FILENAME) cell; \
	chown ron:users archives/$(FILENAME)
	




