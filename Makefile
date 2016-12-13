.PHONY: all install clean dist-clean

CC = gcc
CFLAGS += -Wall -Werror -Wno-deprecated-declarations -pedantic -shared -fPIC -Wl,-z,defs
LDFLAGS += -lbsd

FMLIBS := $(shell pkg-config --cflags --libs libfm)
GTKDIRS := $(shell pkg-config --cflags --libs gtk+-3.0)

all: md5-module.so

md5-module.so: libfm-md5-module.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(GTKDIRS) $(FMLIBS) -o $@ $^ 
clean:
	rm -rf md5-module.so

install: all
	cp md5-module.so /usr/lib/libfm/modules/

dist-clean:
	rm -f /usr/lib/libfm/modules/md5-module.so
