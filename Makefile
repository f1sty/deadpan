DESTDIR = $(HOME)/.local

CFLAGS = -Wall -Wextra -pedantic
TARGET = deadpan
OBJECTS = $(TARGET:=.o)

all: config.h $(TARGET)

$(TARGET): $(OBJECTS)
	gcc -o $@ $<

$(OBJECTS): %.o: %.c
	gcc $(CFLAGS) -c $<

config.h: config.def.h
	cp config.def.h config.h

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(TARGET) $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)

.PHONY: all clean install uninstall
