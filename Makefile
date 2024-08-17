.PHONY: clean install uninstall init

CFLAGS += -Wall -Wextra -pedantic -Wstrict-prototypes -ggdb \
		  -I./include $(FLAGS)

PROFILE := debug

ifeq ($(PROFILE),release)
	CFLAGS += -O3
endif

TARGET_PLATFORM := unix

CFILES=  $(wildcard src/*.c) src/os/$(TARGET_PLATFORM).c
OFILES= $(patsubst %.c,%.o,$(CFILES))

default: libmemalloc.so

libmemalloc.so: $(OFILES)
	@ echo " => libmemalloc.so"
	@ $(CC) $(CCFLAGS) -shared -o ./libmemalloc.so $(OFILES)
	@ echo " => libmemalloc-static.a"
	@ ar rcs libmemalloc-static.a $(OFILES)

.c.o:
	@ echo " CC $@"
	@ $(CC) -c $< -o $@ $(CFLAGS)

INSTALL_PATH ?= /usr/local

install: default
	@ echo "libmemalloc.so => $(INSTALL_PATH)/lib"
	@ echo "libmemalloc-static.a => $(INSTALL_PATH)/lib"
	@ install -d $(INSTALL_PATH)/lib
	@ install -m  644 libmemalloc* $(INSTALL_PATH)/lib
	@ echo "include/memalloc.h => $(INSTALL_PATH)/include"
	@ install -d $(INSTALL_PATH)/include
	@ install -m  644 include/memalloc.h $(INSTALL_PATH)/include

uninstall:
	@ rm -f $(INSTALL_PATH)/lib/libmemalloc* $(INSTALL_PATH)/include/memalloc.h

init:
	@ echo -e \
		"CompileFlags: \n" \
		"Add: [ -I$(shell pwd)/include/ , -xc ]" > .clangd

clean:
	@ rm -f libmemalloc* $(OFILES)
