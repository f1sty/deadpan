DESTDIR = $(HOME)/.local

all: deadpan

deadpan: main.o
	cc -o deadpan main.o

main.o: main.c config.h
	cc -c main.c

clean:
	rm main.o

install: deadpan
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f deadpan $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/deadpan

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/deadpan

.PHONY: all clean install uninstall
