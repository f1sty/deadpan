DESTDIR = $(HOME)/.local

all: dpan

dpan: main.o
	cc -o dpan main.o

main.o: main.c config.h
	cc -c main.c

clean:
	rm main.o

install: dpan
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f dpan $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dpan

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/dpan

.PHONY: all clean install uninstall
