.PHONY: clean install uninstall init test

CFLAGS += -Wall -Wextra -pedantic -Wstrict-prototypes -ggdb \
		  -I./include -fPIC $(FLAGS)

PROFILE := debug

ifeq ($(PROFILE),release)
	CFLAGS += -O3
endif

TARGET_PLATFORM := unix

CFILES=  $(wildcard src/*.c) src/os/$(TARGET_PLATFORM).c
OFILES= $(patsubst %.c,%.o,$(CFILES))

default: memalloc

memalloc: bin/libmemalloc.so

bin/libmemalloc.so: $(OFILES) | bin/
	@ echo " => libmemalloc.so"
	@ $(CC) $(CCFLAGS) -shared -o ./bin/libmemalloc.so $(OFILES)
	@ echo " => libmemalloc.a"
	@ ar rcs bin/libmemalloc.a $(OFILES)

.c.o:
	@ echo " CC $@"
	@ $(CC) -c $< -o $@ $(CFLAGS)

INSTALL_PATH ?= /usr/local

install: default
	@ echo "libmemalloc.so => $(INSTALL_PATH)/lib"
	@ echo "libmemalloc.a => $(INSTALL_PATH)/lib"
	@ install -d $(INSTALL_PATH)/lib
	@ install -m  644 bin/libmemalloc* $(INSTALL_PATH)/lib
	@ echo "memalloc.h => $(INSTALL_PATH)/include"
	@ install -d $(INSTALL_PATH)/include
	@ install -m  644 include/memalloc.h $(INSTALL_PATH)/include

uninstall:
	@ rm -f $(INSTALL_PATH)/lib/libmemalloc* $(INSTALL_PATH)/include/memalloc.h

init:
	@ echo -e \
		"CompileFlags: \n" \
		"Add: [ -I$(shell pwd)/include/ , -xc ]" > .clangd

TESTFILES = $(wildcard tests/*.c)
test: default
	@ $(foreach T,$(TESTFILES), \
	  $(CC) $(CCFLAGS) -o bin/$(patsubst %.c,%.out, $(notdir $(T))) $(T) -Lbin -lmemalloc ; \
	  LD_LIBRARY_PATH=bin bin/$(patsubst %.c,%.out, $(notdir $(T))) || exit 1;)

%/:
	@ mkdir $@

clean:
	@ rm -rf libmemalloc* $(OFILES) bin
