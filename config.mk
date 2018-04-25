# OpenREX version
VERSION = 0.0.1

# Customize below to fit your system

PREFIX = /usr/local

# includes and libs
INCS = -Isrc
LIBS = -lm

# flags
CPPFLAGS = -DVERSION=\"$(VERSION)\"
CFLAGS = $(INCS) $(CPPFLAGS) -std=c99 -pedantic -Wall
LDFLAGS = $(LIBS)
