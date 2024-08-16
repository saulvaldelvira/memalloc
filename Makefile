.PHONY: clean install uninstall

CFLAGS += -Wall -Wextra -pedantic -Wstrict-prototypes -ggdb $(FLAGS)

PROFILE := debug

ifeq ($(PROFILE),release)
	CFLAGS += -O3
endif

CFILES=  $(wildcard *.c)
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

install: edit
	@ echo "libmemalloc.so => $(INSTALL_PATH)/lib"
	@ echo "libmemalloc-static.a => $(INSTALL_PATH)/lib"
	@ install -d $(INSTALL_PATH)/lib
	@ install -m  644 libmemalloc* $(INSTALL_PATH)/lib

uninstall:
	@ rm -f $(INSTALL_PATH)/lib/libmemalloc*

clean:
	@ rm -f libmemalloc* *.o
