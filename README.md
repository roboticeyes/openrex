# OpenREX

OpenREX provides the specification for a binary file format which is used
for the Robotic Eyes REX platform.

This repository contains

* the [complete specification](doc/rex-spec-v1.md)
* native reader/writer
* tools for working with REX files

# Development

In order to compile the code, you need at least a C99 compiler (e.g. gcc, clang)
and a possiblity to work with Makefiles (i.e. make).

## Build

```
mkdir build
cd build
cmake ..
make
make install
```

## Implementation status

* [x] Read file header
* [x] Write file header
* [ ] Read mesh block
* [x] Write mesh block
* [ ] Read material block
* [x] Write material block

* [ ] Read image block
* [ ] Write image block
* [ ] Read lineset block
* [ ] Write lineset block
* [ ] Read vertex block
* [ ] Write vertex block
* [ ] Read text block
* [ ] Write text block
* [ ] Read peoplesimulation block
* [ ] Write peoplesimulation block
* [ ] Read unitypackage block
* [ ] Write unitypackage block
