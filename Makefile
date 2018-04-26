# OpenREX makefile
.POSIX:

include config.mk

BIN = rex-info rex-gencube

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin
TOOLSDIR = tools

SOURCES       := $(wildcard $(SRCDIR)/*.c)
TOOLS_SOURCES := $(wildcard $(TOOLSDIR)/*.c)
INCLUDES      := $(wildcard $(SRCDIR)/*.h)
OBJECTS       := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TOOLS_OBJS    := $(TOOLS_SOURCES:$(TOOLSDIR)/%.c=$(OBJDIR)/%.o)

rex-info: options $(OBJECTS) $(TOOLS_OBJS) $(OBJDIR)/rex-info.o
	$(CC) -o $(BINDIR)/$@ $(OBJECTS) $(OBJDIR)/rex-info.o $(LDFLAGS)
rex-gencube: options $(OBJECTS) $(TOOLS_OBJS) $(OBJDIR)/rex-gencube.o
	$(CC) -o $(BINDIR)/$@ $(OBJECTS) $(OBJDIR)/$@.o $(LDFLAGS)

all: $(BIN)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c config.mk config.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TOOLS_OBJS): $(OBJDIR)/%.o : $(TOOLSDIR)/%.c config.mk config.h
	$(CC) $(CFLAGS) -c $< -o $@

options:
	@echo Build options:
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC      = $(CC)"
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

config.h: src/config.def.h
	test -f src/config.h || cp src/config.def.h src/config.h

clean:
	rm -rf ${BINDIR} ${OBJDIR}
	@echo "Cleanup complete!"

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(BINDIR)/$(BIN) $(DESTDIR)$(PREFIX)/bin
	for f in $(BIN); do chmod 755 "$(DESTDIR)$(PREFIX)/bin/$$f"; done

uninstall:
	for f in $(BIN); do rm -f "$(DESTDIR)$(PREFIX)/bin/$$f"; done

.PHONY: all options clean install uninstall
