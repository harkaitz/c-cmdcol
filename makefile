.SUFFIXES:
.PHONY: all clean install check

PROJECT    =c-cmdcol
VERSION    =1.0.0
CC         =$(TARGET_PREFIX)cc
CFLAGS     = -Wall -g3 -std=c99
PREFIX     =/usr/local
TOOLCHAINS =aarch64-linux-musl x86_64-linux-musl x86_64-w64-mingw32
EXE        =$(HOMEDRIVE:C:=.exe)
DESTDIR    =$(HOMEDRIVE)
PROGS      =cmdcol$(EXE)

all: $(PROGS)
clean:
	rm -f $(PROGS)
install:
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(PROGS) $(DESTDIR)$(PREFIX)/bin
check:
cmdcol$(EXE): cmdcol.c
	$(CC) -o $@ cmdcol.c $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LIBS)
## -- BLOCK:c --
clean: clean-c
clean-c:
	rm -f *.o
## -- BLOCK:c --
## -- BLOCK:man --
install: install-man
install-man:
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1
	cp ./cmdcol.1 $(DESTDIR)$(PREFIX)/share/man/man1
## -- BLOCK:man --
