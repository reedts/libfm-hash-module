.PHONY: all install doc clean dist-clean tags folder

INC_DIR := include
LIB_DIR := lib
OBJ_DIR := obj
SRC_DIR := src

FMLIBS := $(shell pkg-config --cflags --libs libfm)
GTKDIRS := $(shell pkg-config --cflags --libs gtk+-3.0)


CC = gcc
CFLAGS += -Wall -Werror -Wno-deprecated-declarations -pedantic -shared -fPIC\
	  -Wl,-z,defs -I$(INC_DIR) $(FMLIBS) $(GTKDIRS)
LDFLAGS += -lbsd

OBJS := $(OBJ_DIR)/hash-module.o $(OBJ_DIR)/hash.o $(OBJ_DIR)/hash-md5.o


all: folder $(LIB_DIR)/hash-module.so

folder:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(LIB_DIR)

$(LIB_DIR)/hash-module.so: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(FMLIBS) -o $@ $^ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(LIB_DIR)
	rm -rf $(OBJ_DIR)
	$(MAKE) -C doc/latex clean
	rm -rf doc

install: all
	cp $(LIB_DIR)/hash-module.so /usr/lib/libfm/modules/

doc:
	doxygen
	$(MAKE) -C doc/latex

dist-clean: clean
	rm -f /usr/lib/libfm/modules/hash-module.so

tags:
	ctags *.[ch]
