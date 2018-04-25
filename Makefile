# librex
.POSIX:

include config.mk

APPNAME=rex

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES       := $(wildcard $(SRCDIR)/*.c)

INCLUDES      := $(wildcard $(SRCDIR)/*.h)
OBJECTS       := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: options ${APPNAME}

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/$(APPNAME).o: $(APPNAME).c
	$(CC) $(CFLAGS) -c $< -o $@

options:
	@echo ${APPNAME} build options:
	@echo Building for architecture: $(arch)
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC      = $(CC)"
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/db
	mkdir -p $(BINDIR)

config.h:
	cp src/config.def.h src/config.h

$(OBJECTS): config.h config.mk

${APPNAME}: $(OBJECTS) $(OBJDIR)/$(APPNAME).o
	$(CC) -o $(BINDIR)/$@ $(OBJECTS) $(OBJDIR)/$(APPNAME).o $(LDFLAGS)

clean:
	rm -rf ${BINDIR} ${OBJDIR}
	@echo "Cleanup complete!"

install: $(BINDIR)/$(APPNAME)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(BINDIR)/$(APPNAME) $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(APPNAME)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(APPNAME)

.PHONY: all options clean install uninstall
